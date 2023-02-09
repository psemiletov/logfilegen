#include <thread>
#include <chrono>
#include <iostream>


#include "cycle.h"

using namespace std::chrono;

namespace
{
  volatile std::sig_atomic_t g_signal;
}


void f_signal_handler (int signal)
{
  g_signal = signal;
  cout << "Exiting by the signal" << endl;
}


CGenCycle::CGenCycle (CParameters *prms, const string &fname)
{
  params = prms;
  fname_template = fname;
  log_current_size = 0;
  no_free_space = false;
  file_size_total = 0;

  if (params->lines != 0)
     params->duration = 0;

  if (params->size != 0)
     {
      params->duration = 0;
      params->lines = 0;
     }

  logrotator = new CLogRotator (params->logfile, params->max_log_files, string_to_file_size (params->max_log_file_size));
  logrotator->use_gzip = params->use_gzip;

  tpl = new CTpl (fname_template, params->mode);

  std::signal (SIGINT, f_signal_handler);


  if (! params->bstdout)
     {
     //  how many space we occupy with all logfiles?

      size_t free_space = get_free_space (get_file_path (params->logfile));
      size_t size_needed = logrotator->max_log_file_size * logrotator->max_log_files;

      string test_string = tpl->prepare_log_string();
      test_string_size = test_string.size();

      if (params->debug)
          cout << "size_needed, bytes: " << size_needed << endl;

      if (size_needed >= free_space)
         {
          cout << "Output files will not fit to the available disk space with current parameters!" << endl;
          no_free_space = true;
         }
     }

 #ifdef PROM
 exposer = new  Exposer("127.0.0.1:8080");

  // exposer = new  Exposer(params->metrics_addr);
    registry = std::make_shared<Registry>();

#endif
}


CGenCycle::~CGenCycle()
{
  delete tpl;
  delete logrotator;


  #ifdef PROM
delete exposer;

#endif
}


CGenCycleRated::CGenCycleRated (CParameters *prms, const string &fname): CGenCycle (prms, fname)
{
}


bool CGenCycle::open_logfile()
{
  file_out_error = false;

  if (! params->bstdout && ! no_free_space)
     {
      if (file_exists (params->logfile))
         {
          //get current file size
          log_current_size = get_file_size (params->logfile);

          if (params->debug)
             cout << "log_current_size, bytes: " << log_current_size << endl;
         }

      file_out.open (params->logfile, std::ios::app);

      if (file_out.fail())
         {
      //  throw std::ios_base::failure(std::strerror(errno));
          file_out_error = true;
          cout << "cannot create " << params->logfile << "\n";
          return false;
         }
      else
          file_out.exceptions (file_out.exceptions() | std::ios::failbit | std::ifstream::badbit);
     }

   if (no_free_space)
      return false;

   return true;
}


void CGenCycleRated::loop()
{
#ifdef PROM


   // Exposer exposer{"127.0.0.1:8080"};

  // create a metrics registry
  // @note it's the users responsibility to keep the object alive
 // auto registry = std::make_shared<Registry>();

  // add a new counter family to the registry (families combine values with the
  // same name, but distinct label dimensions)
  //
  // @note please follow the metric-naming best-practices:
  // https://prometheus.io/docs/practices/naming/
    auto& counter = BuildCounter()
                             .Name("logfilegen")
                             .Help("Internal counters and stats")
                             .Register(*registry);

    auto& c_lines_counter = counter.Add({{"cycle", "rated"}, {"counter", "lines"}});
    auto& c_bytes_counter = counter.Add({{"cycle", "rated"}, {"counter", "bytes"}});


    auto& gauge = BuildGauge()
                             .Name("logfilegen2")
                             .Help("Internal counters and stats")
                             .Register(*registry);


    auto& g_lines_per_second_gauge = gauge.Add({{"cycle", "rated"}, {"gauge", "lines per second"}});



 //   https://prometheus.io/docs/practices/naming/#labels
 /* auto& http_requests_counter = BuildCounter()
                                    .Name("http_requests_total")
                                    .Help("Number of HTTP requests")
                                    .Register(*registry);
*/
  auto& version_info = BuildInfo()
                           .Name("versions")
                           .Help("Static info about the library")
                           .Register(*registry);
  version_info.Add({{"prometheus", "1.0"}});
  // ask the exposer to scrape the registry on incoming HTTP requests
  exposer->RegisterCollectable (registry);


#endif


   auto start = high_resolution_clock::now();

   size_t lines_counter = 0;

   size_t seconds_counter = 0;
   size_t frame_counter = 0;

   using clock = std::chrono::steady_clock;

   auto next_frame = clock::now();

   while (true)
         {
          next_frame += std::chrono::nanoseconds (1000000000 / params->rate);
          //next_frame += std::chrono::microseconds (1000000 / params->rate);

//          std::cout << "seconds_counter: " << seconds_counter << endl;
  //        std::cout << "frame_counter: " << frame_counter << endl;

          if (frame_counter == params->rate)
             {
              frame_counter = 0;
              seconds_counter++;

             #ifdef PROM

                auto stop = high_resolution_clock::now();
               // auto duration = duration_cast<microseconds>(stop - start);
              auto duration_s = duration_cast<seconds>(stop - start);

               double lines_per_second = (double) lines_counter / duration_s.count();
               g_lines_per_second_gauge.Set (lines_per_second);
             #endif


             }

          frame_counter++;
          lines_counter++;

#ifdef PROM
          c_lines_counter.Increment();
#endif

          if (params->duration != 0) //not endless
          if (params->lines == 0 && seconds_counter == params->duration)
              break;

          if (params->lines != 0 && lines_counter > params->lines)
              break;

          if (params->size != 0 && file_size_total > params->size)
              break;

          if (g_signal == SIGINT)
              break;


          string log_string = tpl->prepare_log_string();

          if (! params->pure)
             {
              #ifdef PROM
               c_bytes_counter.Increment(log_string.size());
              #endif


              if (params->bstdout)
                  cout << log_string << "\n";

              if (! file_out_error && ! no_free_space)
                 {
                  file_out << log_string << "\n";
                  log_current_size += log_string.size();
                  file_size_total += log_string.size();

                  if (log_current_size >= logrotator->max_log_file_size && logrotator->max_log_files > 0)
                     {
                      file_out.close();
                      log_current_size = 0;

                      logrotator->rotate();

                      if (! open_logfile())
                         {
                          cout << "cannot re-open: " << params->logfile << endl;
                          break;
                         }
                     }
                 }
              }
         // std::cout << std::time(0) << endl;

          std::this_thread::sleep_until (next_frame);
         }

  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  auto duration_s = duration_cast<seconds>(stop - start);

  if (params->debug)
     {
      cout << "duration.count (microseconds): " << duration.count() << endl;
      cout << "duration_s.count (seconds): " << duration_s.count() << endl;
     }


  file_out.close();
}


CGenCycleUnrated::CGenCycleUnrated (CParameters *prms, const string &fname): CGenCycle (prms, fname)
{

}


void CGenCycleUnrated::loop()
{
  auto start = high_resolution_clock::now();

  size_t lines_counter = 0;

  // using clock = std::chrono::steady_clock;

  while (true)
        {
         if (g_signal == SIGINT)
             break;

         lines_counter++;

         if (params->lines != 0 && lines_counter > params->lines)
             break;

         if (params->size != 0 && file_size_total > params->size)
             break;

         if (params->duration != 0)
            {
             auto stop = high_resolution_clock::now();
             // auto duration = duration_cast<microseconds>(stop - start);
             auto duration_s = duration_cast<seconds>(stop - start);
             if (duration_s >= chrono::seconds(params->duration))
                break;
            }

//          std::cout << "seconds_counter: " << seconds_counter << endl;
  //        std::cout << "frame_counter: " << frame_counter << endl;


          string log_string = tpl->prepare_log_string();

          if (! params->pure)
             {
              if (params->bstdout)
                 cout << log_string << "\n";

              if (! file_out_error && ! no_free_space)
                 {
                  file_out << log_string << "\n";

                  log_current_size += log_string.size();
                  file_size_total += log_string.size();

                  if (log_current_size >= logrotator->max_log_file_size)
                     {
                      file_out.close();
                      log_current_size = 0;

                      logrotator->rotate();

                      if (! open_logfile())
                         {
                          cout << "cannot re-open: " << params->logfile << endl;
                          break;
                         }
                     }
                 }
              }
         // std::cout << std::time(0) << endl;
          }


  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  auto duration_s = duration_cast<seconds>(stop - start);

  if (params->benchmark)
     {
      double lines_per_second = (double) lines_counter / duration_s.count();
      cout << "Benchmark, lines per seconds: " << lines_per_second << endl;
     }

  if (params->test)
     {
      double lines_per_second = (double) lines_counter / duration_s.count();
      cout << "Test, lines per seconds: " << lines_per_second << endl;
     }

  if (params->stats)
     {
      double lines_per_second = (double) lines_counter / duration_s.count();
      cout << "Statistics, lines per seconds: " << lines_per_second << endl;
     }


  if (params->debug)
     {
      cout << "duration.count (microseconds): " << duration.count() << endl;
      cout << "duration_s.count (seconds): " << duration_s.count() << endl;
     }

  file_out.close();
}
