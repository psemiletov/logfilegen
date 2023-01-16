#include <thread>


#include "cycle.h"


namespace
{
  volatile std::sig_atomic_t g_signal;
}


void f_signal_handler (int signal)
{
  g_signal = signal;
  cout << "SIGNAL!!!" << endl;
}



void CParameters::print()
{

  cout << "------------ Print parameters -------------" << endl;


  cout << "duration: " << duration << endl;
  cout << "rate: " << rate << endl;
  cout << "templatefile: " << templatefile << endl;
  cout << "logfile: " << logfile << endl;

  cout << "max_log_files: " << max_log_files << endl;
  cout << "logsize: " << max_log_file_size << endl;


  cout << "pure: " << pure << endl;

  cout << "mode: " << mode << endl;
  cout << "debug: " << debug << endl;
  cout << "stdout: " << bstdout << endl;

  cout << "------------ **************** -------------" << endl;
}



CGenCycle::CGenCycle (CParameters *prms, const string &fname)
{
  params = prms;
  fname_template = fname;
  log_current_size = 0;
  no_free_space = false;

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

     //  cout << "test_string_size, bytes: " << test_string_size  << endl;

      if (params->debug)
         cout << "size_needed, bytes: " << size_needed << endl;

      if (size_needed >= free_space)
        {
         //exit from program

         cout << "Output files will not fit to the available disk space with current parameters, exiting!" << endl;
         no_free_space = true;
         //return;
        }
     }

}


CGenCycle::~CGenCycle()
{
  delete tpl;
  delete logrotator;
}


CGenCycleUnrated::CGenCycleUnrated (CParameters *prms, const string &fname): CGenCycle (prms, fname)
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

  //   file_out.open (params->logfile, std::ios::out | std::ios::ate);

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

   return true;
}


void CGenCycleUnrated::loop()
{
   auto start = high_resolution_clock::now();

   int seconds_counter = 0;
   int frame_counter = 0;

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
             }


          frame_counter++;

          if (seconds_counter == params->duration)
              break;

          if (g_signal == SIGINT)
             {
          //    cout << "break the main loop" << endl;
              break;
             }


          string log_string = tpl->prepare_log_string();

          if (! params->pure)
             {
              if (params->bstdout)
                 cout << log_string << "\n";

              if (! file_out_error && ! no_free_space)
                {
                 file_out << log_string << "\n";
                 log_current_size += log_string.size();
                 //log_current_size += test_string_size;

                 if (log_current_size >= logrotator->max_log_file_size)
                    {
                   // if (params->debug)
                     //   cout << "ROTATE" << endl;

                     //begin rotate:
                     //close file
                      file_out.close();
                      log_current_size = 0;

                     //rotate
                      logrotator->rotate();

                     //open new file to write
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
