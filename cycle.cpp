#include <thread>
#include <chrono>

#include <iostream>
#include <string.h>


#include "cycle.h"

using namespace std;
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
 exposer = new  Exposer(params->metrics_addr);
  // exposer = new  Exposer(params->metrics_addr);
    registry = std::make_shared<Registry>();
#endif

 //SERV


#if defined(_WIN32) || defined(_WIN64)
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("\nError: Windows socket subsytsem could not be initialized. Error Code: %d. Exiting..\n", WSAGetLastError());
        //exit(1);
    }
#endif

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        cout << "ERROR opening socket" << endl;

//     bzero((char *) &serv_addr, sizeof(serv_addr));

     memset (&serv_addr, 0, sizeof(serv_addr));

//     portno = 8888;
      portno = stoi(params->port.c_str());;


     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);

     int retcode = ::bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

     if ( retcode == 0)
        {
         listen(sockfd,5);
        server_run = true;
         }
       else
          cout << "ERROR on binding" << endl;

     //f_handle = async(launch::async,&CGenCycle::server_handle,this);



         //response = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: @clen\n\n<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\r\n<title>logfilegen metrics</title>\r\n</head>\r\n<body>\r\n@body</body>\r\n</html>";


     response = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: @clen\n\n<html>\r\n<head>\r\n<title>logfilegen metrics</title>\r\n</head>\r\n<body>\r\n@body</body>\r\n</html>";



   th_srv = new thread (&CGenCycle::server_handle, this);
   th_srv->detach();
}

//http://localhost:8888/metrics

void CGenCycle::server_handle()
{
 while (server_run)
 {
  //  cout << "void CGenCycle::server_handle()" << endl;


   clilen = sizeof(cli_addr);

     newsockfd = accept(sockfd,
                 (struct sockaddr *) &cli_addr,
                 &clilen);

     if (newsockfd < 0)
          cout << ("ERROR on accept");

     //bzero(buffer,256);

     memset (&buffer, 0, 256);



     int n = read(newsockfd,buffer,255);

     if (n < 0)
        cout <<  "ERROR reading from socket" << endl;

    printf("Here is the message: %s\n",buffer);


    string request (buffer);
    string rsp;
    string body;
    body += "<p>";
    body += "seconds_counter:";
    body += to_string (seconds_counter);
    body += "</p>";


    if (request.find ("GET /metrics") != string::npos)
       {
    //    rsp = str_replace (response, "@body", body);
    //    rsp = str_replace (response, "@clen", to_string(body.size()));
//        cout << rsp << endl;
      //  n = write(newsockfd,rsp.c_str(),rsp.size());


        string ts = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:" + to_string(body.size()) + "\n\n" +body;
          n = write(newsockfd,ts.c_str(),ts.size());


         if (n < 0)
           cout << "ERROR writing to socket" << endl;

       }


//     n = write(newsockfd,"I got your message",18);

//OK
//    string ts = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: 16\n\n<h1>testing</h1>";
  //  n = write(newsockfd,ts.c_str(),ts.size());


     //n = write(newsockfd,rsp.c_str(),rsp.size());




    close(newsockfd);


 }



}



CGenCycle::~CGenCycle()
{
  server_run = false;
       close(sockfd);


delete th_srv;


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

             #ifdef PROM
              if (params->metrics && seconds_counter)
              {
                auto stop = high_resolution_clock::now();
               // auto duration = duration_cast<microseconds>(stop - start);
              auto duration_s = duration_cast<seconds>(stop - start);

               lines_per_second = (double) lines_counter / duration_s.count();
               g_lines_per_second_gauge.Set (lines_per_second);
              }
             #endif


             }

          frame_counter++;
          lines_counter++;

#ifdef PROM
            if (params->metrics  && seconds_counter)
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
                  if (params->metrics  && seconds_counter)

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


  server_run = false;


  file_out.close();
}


CGenCycleUnrated::CGenCycleUnrated (CParameters *prms, const string &fname): CGenCycle (prms, fname)
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

 // size_t lines_counter = 0;

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
             if (duration_s >= chrono::seconds(params->duration))
                break;
            }



//          std::cout << "seconds_counter: " << seconds_counter << endl;
  //        std::cout << "frame_counter: " << frame_counter << endl;


          string log_string = tpl->prepare_log_string();

          if (! params->pure)
             {
            #ifdef PROM
              if (params->metrics)
               {
               auto stop = high_resolution_clock::now();
               auto duration_s = duration_cast<seconds>(stop - start);

              if (duration_s.count())
               {
                c_lines_counter.Increment();

                c_bytes_counter.Increment(log_string.size());


                lines_per_second = (double) lines_counter / duration_s.count();
                g_lines_per_second_gauge.Set (lines_per_second);
              }
              }
             #endif





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
      lines_per_second = (double) lines_counter / duration_s.count();
      cout << "Benchmark, lines per seconds: " << lines_per_second << endl;
     }

  if (params->test)
     {
      lines_per_second = (double) lines_counter / duration_s.count();
      cout << "Test, lines per seconds: " << lines_per_second << endl;
     }

  if (params->stats)
     {
      lines_per_second = (double) lines_counter / duration_s.count();
      cout << "Statistics, lines per seconds: " << lines_per_second << endl;
     }


  if (params->debug)
     {
      cout << "duration.count (microseconds): " << duration.count() << endl;
      cout << "duration_s.count (seconds): " << duration_s.count() << endl;
     }

  file_out.close();
}
