#include <thread>
#include <chrono>
//#include <future>

#include <iostream>
#include <string.h>


#ifndef PROM

//#include <arpa/inet.h>

#endif

#include "cycle.h"

using namespace std::chrono;

namespace
{
  volatile std::sig_atomic_t g_signal;
}


void f_signal_handler (int signal)
{
  g_signal = signal;

  std::cout << "Exiting by the signal" << std::endl;
}


CGenCycle::CGenCycle (CParameters *prms, const std::string &fname)
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

      std::string test_string = tpl->prepare_log_string();
      test_string_size = test_string.size();

      if (params->debug)
          std::cout << "size_needed, bytes: " << size_needed << std::endl;

      if (size_needed >= free_space)
         {
          std::cout << "Output files will not fit to the available disk space with current parameters!" << std::endl;
          no_free_space = true;
         }
     }

 #ifdef PROM
  exposer = new Exposer (params->metrics_addr);
  registry = std::make_shared<Registry>();
#endif

 //SERV

  #ifndef PROM


 if (params->metrics)
 {
#if defined(_WIN32) || defined(_WIN64)
  WSADATA wsa;
  if (WSAStartup (MAKEWORD(2,2),&wsa) != 0)
     {
      printf("Error: Windows socket subsystem could not be initialized. Error Code: %d\n", WSAGetLastError());
     }
#endif

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
     std::cout << "ERROR opening socket" << std::endl;

   int yes=1;
//char yes='1'; // use this under Solaris

if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
{
    perror("setsockopt");
  //  exit(1);
}


  memset (&serv_addr, 0, sizeof(serv_addr));

  portno = std::stoi(params->port.c_str());;

  serv_addr.sin_family = AF_INET;

  serv_addr.sin_addr.s_addr = INADDR_ANY;

//  inet_pton (AF_INET, params->ip.c_str(), &serv_addr);


  serv_addr.sin_port = htons(portno);


  int retcode = ::bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

  if (retcode == 0)
     {
      listen(sockfd,5);
      server_run = true;
     }
  else
      std::cout << "ERROR on binding" << std::endl;

   // response = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: @clen\n\n<html>\r\n<head>\r\n<title>logfilegen metrics</title>\r\n</head>\r\n<body>\r\n@body</body>\r\n</html>";

 //  th_srv = new std::thread (&CGenCycle::server_handle, this);
 //  th_srv->detach();


    f_handle = std::async(std::launch::async, &CGenCycle::server_handle, this);

//        auto f = std::async(&CGenCycle::server_handle, this);

 }

#endif
}

//http://localhost:8888/metrics

#ifndef PROM


void CGenCycle::server_handle()
{
 while (server_run)
 {
  //  std::cout << "void CGenCycle::server_handle()" << std::endl;


   clilen = sizeof(cli_addr);

     newsockfd = accept(sockfd,
                 (struct sockaddr *) &cli_addr,
                 &clilen);

     if (newsockfd < 0)
       {
       // std::cout << "ERROR on accept" << std::endl;
        return;
      }

     //bzero(buffer,256);

     memset (&buffer, 0, 256);



     int n = read(newsockfd,buffer,255);

     if (n < 0)
       {
//        std::cout <<  "ERROR reading from socket" << std::endl;
        return;
       }

 //   printf("Here is the message: %s\n",buffer);


    std::string request (buffer);
    std::string rsp;

    if (request.find ("GET /stats") != std::string::npos)
       {
            std::string body;


    body += "logstring:";
    body += tpl->vars["$logstring"]->get_val();
    body += "\n<br>";
    body += "file_size_total:";
    body += std::to_string (file_size_total);
    body += "\n<br>";
    body += "seconds_counter:";
    body += std::to_string (seconds_counter_ev);
    body += "\n<br>";
    body += "lines_counter:";
    body += std::to_string (lines_counter);
    body += "\n<br>";
    body += "lines_per_second:";
    body += std::to_string (lines_per_second);



        std::string ts = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:" + std::to_string(body.size()) + "\n\n" +body;
             n = write(newsockfd,ts.c_str(),ts.size());

         if (n < 0)
           std::cout << "ERROR writing to socket" << std::endl;
       }

    if (request.find ("GET /metrics") != std::string::npos)
       {
    std::string body;
/*
# HELP go_memstats_mcache_inuse_bytes Number of bytes in use by mcache structures.
# TYPE go_memstats_mcache_inuse_bytes gauge
go_memstats_mcache_inuse_bytes 9600
 */

   //  body += "# HELP logstring shows current template log string";
   //  body += "# TYPE logstring";

  //  body += "logstring ";
 //   body += tpl->vars["$logstring"]->get_val();
 //   body += "\n";
     body += "# HELP seconds_counter shows how many seconds past\n";
     body += "# TYPE seconds_counter counter\n";
     body += "seconds_counter ";
    body += std::to_string (seconds_counter_ev);
    body += "\n";


   // body += "file_size_total ";
  //  body += std::to_string (file_size_total);
  //  body += "\n";

   body += "# HELP lines_counter shows how many lines has been generated\n";
     body += "# TYPE lines_counter counter\n";
     body += "lines_counter ";
    body += std::to_string (lines_counter);
    body += "\n";

       body += "# HELP lines_per_second shows the average lines per second rate\n";
     body += "# TYPE lines_per_second gauge\n";
     body += "lines_per_second  ";
    body += std::to_string (lines_per_second );
    body += "\n";



          std::string ts = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length:" + std::to_string(body.size()) + "\n\n" +body;
             n = write(newsockfd,ts.c_str(),ts.size());

         if (n < 0)
           std::cout << "ERROR writing to socket" << std::endl;
       }



    shutdown(newsockfd, 2);
    close(newsockfd);


 }

}

#endif


CGenCycle::~CGenCycle()
{
 #ifndef PROM


 if (params->metrics)
 {

  server_run = false;
  //close(sockfd);

  shutdown(sockfd, 2);
  close(sockfd);



  delete tpl;
  delete logrotator;
 }

#endif

  #ifdef PROM
delete exposer;

#endif
}


CGenCycleRated::CGenCycleRated (CParameters *prms, const std::string &fname): CGenCycle (prms, fname)
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
             std::cout << "log_current_size, bytes: " << log_current_size << std::endl;
         }

      file_out.open (params->logfile, std::ios::app);

      if (file_out.fail())
         {
      //  throw std::ios_base::failure(std::strerror(errno));
          file_out_error = true;
          std::cout << "cannot create " << params->logfile << "\n";
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

 auto& counter = BuildCounter()
                             .Name("data_generated_total")
                             .Help("Internal counters and stats")
                             .Register(*registry);

    auto& c_lines_counter = counter.Add({{"cycle", "rated"}, {"counter", "lines generated"}});
    auto& c_bytes_counter = counter.Add({{"cycle", "rated"}, {"counter", "bytes generated"}});


    auto& gauge = BuildGauge()
                             .Name("data_generated_current")
                             .Help("Internal counters and stats")
                             .Register(*registry);


    auto& g_lines_per_second_gauge = gauge.Add({{"cycle", "rated"}, {"gauge", "lines per second"}});

    auto& version_info = BuildInfo()
                           .Name("logstring_template")
                           .Help("Shows the logstring template in use")
                           .Register(*registry);

         version_info.Add({{"logstring", tpl->vars["$logstring"]->get_val()}});
  // ask the exposer to scrape the registry on incoming HTTP requests

  if (params->metrics)
  exposer->RegisterCollectable (registry);


#endif


   auto start = high_resolution_clock::now();


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

            if (params->metrics)
              {
                auto stop = high_resolution_clock::now();
                auto duration_s = duration_cast<seconds>(stop - start);
                seconds_counter_ev = duration_s.count();

                if (duration_s.count())
                   lines_per_second = (double) lines_counter / duration_s.count();

               #ifdef PROM
                g_lines_per_second_gauge.Set (lines_per_second);
               #endif
              }


             }

          frame_counter++;
          lines_counter++;

#ifdef PROM
            if (params->metrics)
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


          std::string log_string = tpl->prepare_log_string();

          if (! params->pure)
             {
              #ifdef PROM
                  if (params->metrics)
                      c_bytes_counter.Increment(log_string.size());
              #endif


              if (params->bstdout)
                  std::cout << log_string << "\n";

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
                          std::cout << "cannot re-open: " << params->logfile << std::endl;
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
      std::cout << "duration.count (microseconds): " << duration.count() << std::endl;
      std::cout << "duration_s.count (seconds): " << duration_s.count() << std::endl;
     }


  server_run = false;


  file_out.close();
}


CGenCycleUnrated::CGenCycleUnrated (CParameters *prms, const std::string &fname): CGenCycle (prms, fname)
{

}


void CGenCycleUnrated::loop()
{
#ifdef PROM


    auto& counter = BuildCounter()
                             .Name("data_generated_total")
                             .Help("Internal counters and stats")
                             .Register(*registry);

    auto& c_lines_counter = counter.Add({{"cycle", "unrated"}, {"counter", "lines generated"}});
    auto& c_bytes_counter = counter.Add({{"cycle", "unrated"}, {"counter", "bytes generated"}});


    auto& gauge = BuildGauge()
                             .Name("data_generated_current")
                             .Help("Internal counters and stats")
                             .Register(*registry);


    auto& g_lines_per_second_gauge = gauge.Add({{"cycle", "unrated"}, {"gauge", "lines per second"}});

    auto& version_info = BuildInfo()
                           .Name("logstring_template")
                           .Help("Shows the logstring template in use")
                           .Register(*registry);

         version_info.Add({{"logstring", tpl->vars["$logstring"]->get_val()}});
  // ask the exposer to scrape the registry on incoming HTTP requests

  if (params->metrics)
      exposer->RegisterCollectable (registry);


#endif



   auto start = high_resolution_clock::now();

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
             auto duration_s = duration_cast<seconds>(stop - start);
             if (duration_s >= std::chrono::seconds(params->duration))
                break;
            }

//          std::cout << "seconds_counter: " << seconds_counter << endl;
  //        std::cout << "frame_counter: " << frame_counter << endl;


          std::string log_string = tpl->prepare_log_string();

          if (! params->pure)
             {
              if (params->metrics)
               {
                 auto stop = high_resolution_clock::now();
                 auto duration_s = duration_cast<seconds>(stop - start);

                if (duration_s.count())
                   {

                   seconds_counter_ev = duration_s.count();

                   lines_per_second = (double) lines_counter / duration_s.count();
                  #ifdef PROM

                    c_lines_counter.Increment();
                    c_bytes_counter.Increment(log_string.size());

                   g_lines_per_second_gauge.Set (lines_per_second);

                   #endif

                   }
               }





              if (params->bstdout)
                 std::cout << log_string << "\n";

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
                          std::cout << "cannot re-open: " << params->logfile << std::endl;
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
      lines_per_second = (double) lines_counter / duration_s.count();
      std::cout << "Benchmark, lines per seconds: " << lines_per_second << std::endl;
     }

  if (params->test)
     {
      lines_per_second = (double) lines_counter / duration_s.count();
      std::cout << "Test, lines per seconds: " << lines_per_second << std::endl;
     }

  if (params->stats)
     {
      lines_per_second = (double) lines_counter / duration_s.count();
      std::cout << "Statistics, lines per seconds: " << lines_per_second << std::endl;
     }


  if (params->debug)
     {
      std::cout << "duration.count (microseconds): " << duration.count() << std::endl;
      std::cout << "duration_s.count (seconds): " << duration_s.count() << std::endl;
     }

  file_out.close();
}
