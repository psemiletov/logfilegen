#include <thread>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string.h>
#include <csignal>
#include <fstream>
#include <unistd.h>


#ifdef _WIN32
#define clrscr() system("cls")
#else
#include <stdio.h>
#define clrscr() printf("\e[1;1H\e[2J")
#endif

#ifndef PROM
//#include <arpa/inet.h>
#endif

#include "cycle.h"

using namespace std::chrono;


std::mutex rotation_mutex;

namespace
{
  volatile std::sig_atomic_t g_signal;
  volatile int sockfd;
}



void do_task (CGenCycle *c)
{
  c->loop();
}


void f_signal_handler (int signal)
{
  g_signal = signal;

  shutdown (sockfd, 2);
  close (sockfd);

  std::cout << "Exiting by the signal" << std::endl;
}


CGenCycle::CGenCycle (CProducer *prod, CParameters *prms, const std::string &fname)
{
  params = prms;
  producer = prod;



//  std::signal (SIGINT, f_signal_handler);


 }



//http://localhost:8888/metrics

#ifndef PROM

void CProducer::server_handle()
{
  while (server_run)
        {
         //  std::cout << "void CGenCycle::server_handle()" << std::endl;

         clilen = sizeof (cli_addr);

         newsockfd = accept (sockfd, (struct sockaddr *) &cli_addr, &clilen);

         if (newsockfd < 0)
            {
//             std::cout << "ERROR on accept" << std::endl;
             return;
            }

         memset (&buffer, 0, 256);

         int n = read (newsockfd, buffer, 255);

         if (n < 0)
            {
             std::cout <<  "ERROR reading from socket" << std::endl;
             return;
            }


         std::string request (buffer);
         std::string rsp;

         if (request.find ("GET /metrics") != std::string::npos)
            {
             std::string body;

             body += "# HELP seconds_counter shows how many seconds past\n";
             body += "# TYPE seconds_counter counter\n";
             body += "seconds_counter ";
             body += std::to_string (seconds_counter_ev);
             body += "\n";

             body += "# HELP lines_counter shows how many lines has been generated\n";
             body += "# TYPE lines_counter counter\n";
             body += "lines_counter ";
             body += std::to_string (lines_counter);
             body += "\n";

             body += "# HELP lines_per_second shows the average lines per second rate\n";
             body += "# TYPE lines_per_second gauge\n";
             body += "lines_per_second ";
             body += std::to_string (round (lines_per_second));
             body += "\n";

             body += "# HELP bytes_per_second shows the average bytes per second rate\n";
             body += "# TYPE bytes_per_second gauge\n";
             body += "bytes_per_second ";
             body += std::to_string (round (bytes_per_second));
             body += "\n";

             std::string ts = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length:" + std::to_string(body.size()) + "\n\n" +body;
             n = ::write (newsockfd, ts.c_str(), ts.size());
             if (n < 0)
                 std::cout << "ERROR writing to socket" << std::endl;
            }
         else
             if (request.find ("GET /") != std::string::npos)
                {
                 std::string body = "<!doctype html>\r\n<html>\r\n<head>\r\n<meta charset=\"UTF-8\"><meta http-equiv=\"refresh\" content=\"@s\" >\r\n<title>logfilegen</title>\r\n</head>\r\n<body>@b\r\n</body>\r\n</html>";


                 std::string t;

                 t += "<table>\r\n";

                 t += "<tr>\r\n";
                 t += "<td><b>Metric</td>";
                 t += "<td><b>Raw</b></td>";
                 t += "<td><b>Adapted</b></td>";
                 t += "</tr>\r\n";


                 t += "<tr>\r\n";

                 t += "<td>seconds_counter:</td>";
                 t += "<td>";
                 t += std::to_string (seconds_counter_ev);
                 t += "</td>";
                 t += "<td>";
                 t += format3 (seconds_counter_ev);
                 t += "</td>";

                 t += "</tr>\r\n";


                 t += "<tr>\r\n";

                 t += "<td>file_size_total</td>";
                 t += "<td>";
                 t += bytes_to_file_size (file_size_total);
                 t += "</td>";
                 t += "<td>";
                 t += bytes_to_file_size3 (file_size_total);
                 t += "</td>";

                 t += "</tr>\r\n";


                 t += "<tr>\r\n";

                 t += "<td>lines_counter</td>";
                 t += "<td>";
                 t += std::to_string (lines_counter);
                 t += "</td>";
                 t += "<td>";
                 t += format3 (lines_counter);
                 t += "</td>";

                 t += "</tr>\r\n";


                 t += "<tr>\r\n";

                 t += "<td>lines_per_second</td>";
                 t += "<td>";
                 t += std::to_string ((int)round (lines_per_second));
                 t += "</td>";
                 t += "<td>";
                 t += format3 (lines_per_second);
                 t += "</td>";

                 t += "</tr>\r\n";

                 t += "<tr>\r\n";

                 t += "<td>bytes_per_second:</td>";
                 t += "<td>";
                 t += std::to_string ((int)round (bytes_per_second));
                 t += "</td>";
                 t += "<td>";
                 t += format3 (round (bytes_per_second));
                 t += "</td>";

                 t += "</tr>\r\n";

                 t += "</table>\r\n";

//                 t += "<v>logstring:</b>";
  //               t += tpl->vars["$logstring"]->get_val();


                 str_replace (body, "@b", t);
                 str_replace (body, "@s", std::to_string (params->poll));

                 std::string ts = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:" + std::to_string (body.size()) + "\n\n" +body;

                 n = ::write (newsockfd, ts.c_str(), ts.size());
                 if (n < 0)
                     std::cout << "ERROR writing to socket" << std::endl;
                }

    shutdown (newsockfd, 2);
    close (newsockfd);
   }
}

#endif


CGenCycle::~CGenCycle()
{

// delete tpl;
}


CGenCycleRated::CGenCycleRated (CProducer *prod, CParameters *prms, const std::string &fname): CGenCycle (prod, prms, fname)
{
}


bool CProducer::open_logfile()
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


   //   file_out.rdbuf()->pubsetbuf(bf, sizeof (bf));


      file_out.open (params->logfile, std::ios::app /*| std::ios::ios_base::binary*/);

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

   auto start = high_resolution_clock::now();

   using clock = std::chrono::steady_clock;

   auto next_frame = clock::now();

   while (true)
         {
          if (g_signal == SIGINT)
              break;

          next_frame += std::chrono::nanoseconds (1000000000 / params->rate);

          if (frame_counter == params->rate)
             {
              frame_counter = 0;
              producer->seconds_counter++;

              if (params->metrics)
                 {
                  producer->stop = high_resolution_clock::now();
                  auto duration_s = duration_cast<seconds>(producer->stop - producer->start);
                  producer->seconds_counter_ev = duration_s.count();

                  if (producer->seconds_counter)
                     {
                      producer->bytes_per_second = (double) producer->file_size_total / producer->seconds_counter_ev;
                      producer->lines_per_second = (double) producer->lines_counter / duration_s.count();
                     }

                 }
             }

         frame_counter++;

         std::string log_string = producer->tpl->prepare_log_string();
         producer->write (log_string, true);

          std::this_thread::sleep_until (next_frame);
        }


}


CGenCycleUnrated::CGenCycleUnrated (CProducer *prod, CParameters *prms, const std::string &fname): CGenCycle (prod, prms, fname)
{

}


void CGenCycleUnrated::loop()
{


  //auto start = high_resolution_clock::now();

  // using clock = std::chrono::steady_clock;

  while (g_signal != SIGINT)
        {
         //if (g_signal == SIGINT)
           //  break;

         std::string log_string = producer->tpl->prepare_log_string();

         producer->write (log_string, false);
        }

}


void CProducer::write_results()
{

  time_t rawtime;
  struct tm * timeinfo;

  time (&rawtime);
  timeinfo = localtime (&rawtime);

  char datebuffer [128];
  strftime (datebuffer, 128, "%d/%b/%Y:%H:%M:%S %z", timeinfo);

  lines_per_second = 0;

  if (seconds_counter_ev > 0)
     lines_per_second = (double) lines_counter / seconds_counter_ev;
  else
      std::cout << "cannot divide by zero and evaluate lines_per_second\n";

  std::string fdate = datebuffer;
  std::string fduration = std::to_string (seconds_counter_ev);
  std::string fmode = params->mode;
  //std::string ftemplate = tpl->vars["$logstring"]->get_val();
  std::string fsize_generated = std::to_string (file_size_total);
  std::string flines_generated = std::to_string (lines_counter);
  std::string flpsperformance = std::to_string (round (lines_per_second));
  std::string fbpsperformance = std::to_string (round (bytes_per_second));

  std::string st = params->results_template;

  str_replace (st, "@date", fdate);
  str_replace (st, "@duration", fduration);
  str_replace (st, "@mode", fmode);
  str_replace (st, "@template", params->templatefile);
  str_replace (st, "@size_generated", fsize_generated);
  str_replace (st, "@lines_generated", flines_generated);
  str_replace (st, "@performance_lps", flpsperformance);
  str_replace (st, "@performance_bps", fbpsperformance);

  if (params->results == "stdout")
     std::cout << st << std::endl;
  else
      if (path_exists (params->results))
          string_save_to_file (params->results, st);

}


void CProducer::write (const std::string &s, bool rated)
{
 std::lock_guard<std::mutex> coutLock(rotation_mutex);

  lines_counter++;

  //rated
  if (rated)
  if (params->duration != 0) //not endless
    {
       if (params->lines == 0 && seconds_counter == params->duration)
          {
           g_signal = SIGINT;
           return;
          }
    }

//unrated
  if (! rated)
  if (params->duration != 0)
            {
             stop = high_resolution_clock::now();
             auto duration_s = duration_cast<seconds>(stop - start);
             if (duration_s >= std::chrono::seconds(params->duration))
                {
                 g_signal = SIGINT;
                 return;
                 }
            }


  if (params->lines != 0 && lines_counter > params->lines)
     {
      g_signal = SIGINT;
      return;
     }

  if (params->size != 0 && file_size_total > params->size)
     {
      g_signal = SIGINT;
      return;
     }




  if (params->bstdout)
     {
      if (lines_counter % 24 == 0)
          clrscr();

      std::cout << s << "\n";
      return;
     }


  if (! file_out_error && ! no_free_space)
     {
      file_out << s << "\n";

      log_current_size += s.size();
      file_size_total += s.size();

      if (log_current_size >= logrotator->max_log_file_size)
         {
          file_out.close();
          log_current_size = 0;

          logrotator->rotate();

          if (! open_logfile())
             {
              std::cout << "cannot re-open: " << params->logfile << std::endl;

              g_signal = SIGINT;

              shutdown (sockfd, 2);
              close (sockfd);
             }
         }
     }
}




CProducer::CProducer (CParameters *prms, const std::string &fname)
{

  params = prms;
  tpl = new CTpl (fname, params->mode);


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

   std::signal (SIGINT, f_signal_handler);


  if (! params->bstdout)
     {
     //  how many space we occupy with all logfiles?

      std::string fpath = get_file_path (params->logfile);
      size_t free_space = 0;
      if (file_exists(fpath))
          free_space = get_free_space (fpath);

      size_t size_needed = logrotator->max_log_file_size * logrotator->max_log_files;

     // std::string test_string = tpl->prepare_log_string();
     // test_string_size = test_string.size();

      if (params->debug)
          std::cout << "size_needed, bytes: " << size_needed << std::endl;

      if (size_needed >= free_space)
         {
          std::cout << "Output files will not fit to the available disk space with current parameters!" << std::endl;
          no_free_space = true;
         }
     }


 //SERV


 if (params->metrics)
    {
#if defined(_WIN32) || defined(_WIN64)
     WSADATA wsa;
     if (WSAStartup (MAKEWORD(2,2),&wsa) != 0)
         printf("Error: Windows socket subsystem could not be initialized. Error Code: %d\n", WSAGetLastError());
#endif

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        std::cout << "ERROR opening socket" << std::endl;


#if defined(_WIN32) || defined(_WIN64)

     char yes='1'; // use this under Solaris and WIN
     if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
        perror("setsockopt");

#else

     int yes=1;
     if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
        perror("setsockopt");

#endif

     memset (&serv_addr, 0, sizeof (serv_addr));

     portno = std::stoi(params->port.c_str());;
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
    //  inet_pton (AF_INET, params->ip.c_str(), &serv_addr);
     serv_addr.sin_port = htons(portno);

     int retcode = ::bind (sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr));

     if (retcode == 0)
        {
         listen (sockfd, 5);
         server_run = true;
        }
    else
        std::cout << "ERROR on binding" << std::endl;

 }

}

//http://localhost:8888/metrics






CProducer::~CProducer()
{

 if (params->metrics)
    {
     server_run = false;
     shutdown (sockfd, 2);
     close (sockfd);
    }

 delete tpl;
 delete logrotator;
}



void CProducer::run()
{

   std::string test_string = tpl->prepare_log_string();
   test_string_size = test_string.size();


   /*auto */start = high_resolution_clock::now();


   if (params->rate == 0)
     {
      /*

      cycle = new CGenCycleUnrated (this, params, fname_template);
      if (open_logfile())
          cycle->loop();

        */


            if (! open_logfile())
               return;


        CGenCycleUnrated *c1 = new CGenCycleUnrated (this, params, fname_template);
        CGenCycleUnrated *c2 = new CGenCycleUnrated (this, params, fname_template);

          std::thread t1;
          std::thread t2;

          t1 = std::thread(&do_task, c1);
          t2 = std::thread(&do_task, c2);


     t1.join();
     t2.join();


    delete c1;
    delete c2;



/*
      for (size_t i = 0; i < threads_count; i++)
          {

          }
*/

     }
  else
      {
      cycle = new CGenCycleRated (this, params, fname_template);
      if (open_logfile())
          cycle->loop();
     }


  /*auto */stop = high_resolution_clock::now();

  auto duration_s = duration_cast<seconds>(stop - start);
  seconds_counter_ev = duration_s.count();

  server_run = false;


   if (params->benchmark && seconds_counter_ev)
     {
      lines_per_second = (double) lines_counter / seconds_counter_ev;
      std::cout << "Benchmark, lines per seconds: " << lines_per_second << std::endl;
     }

  if (params->test && seconds_counter_ev)
     {
      lines_per_second = (double) lines_counter / seconds_counter_ev;
      std::cout << "Test, lines per seconds: " << lines_per_second << std::endl;
     }

  if (! params->results.empty())
      write_results();

  if (! params->results.empty())
      write_results();

  file_out.close();

}
