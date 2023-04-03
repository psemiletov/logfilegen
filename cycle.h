#ifndef CYCLE_H
#define CYCLE_H

#include <fstream>

#ifndef PROM
#include <future>
#endif


#if defined(_WIN32) || defined(_WIN64)
//#include <WinSock2.h>
//#include <WinSock.h>
#include <winsock2.h>
#endif

#ifdef __HAIKU__
#include <Socket.h>
#endif


#if defined (__unix__) || defined (__APPLE__)
#include <netinet/in.h>
#include <sys/socket.h>
//#include <arpa/inet.h>
#endif






#include "pairfile.h"
#include "utl.h"
#include "tpl.h"
#include "logrot.h"
#include "params.h"

class CProducer;


class CGenCycle
{
public:


  CProducer *producer;
  CParameters *params;
//  CTpl *tpl;

  ///////// stats
//  double bytes_per_second;
//  double lines_per_second;
//  size_t file_size_total;
//  size_t lines_counter_last = 0;
  //size_t lines_counter = 0;
  //size_t seconds_counter_ev = 0;
 // size_t seconds_counter = 0;
  size_t frame_counter = 0;
  //////


  CGenCycle (CProducer *prod, CParameters *prms, const std::string &fname);

  //bool open_logfile();

  virtual ~CGenCycle();
  virtual void loop() = 0;

};


class CGenCycleRated: public CGenCycle
{
public:

  CGenCycleRated (CProducer *prod, CParameters *prms, const std::string &fname);
  void loop();

};


class CGenCycleUnrated: public CGenCycle
{
public:

  CGenCycleUnrated (CProducer *prod, CParameters *prms, const std::string &fname);
  void loop();

};


class CProducer
{
public:

    CTpl *tpl;


   ///SERV

//  std::chrono::_V2::system_clock start;
//  std::chrono::_V2::steady_clock clock;

  std::chrono::time_point<std::chrono::high_resolution_clock> start;
  std::chrono::time_point<std::chrono::high_resolution_clock> stop;


  int newsockfd, portno;

#if defined(_WIN32) || defined(_WIN64)
  int clilen;
#else
  socklen_t clilen;
#endif

  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  bool server_run;
  std::future<void> f_handle;
   //  std::thread *th_srv;

  std::string response;

/*
   static std::atomic<size_t> lines_counter;
   static std::atomic<size_t> lines_counter_last;

   static std::atomic<double> bytes_per_second;
   static std::atomic<double> lines_per_second;
   static std::atomic<size_t> file_size_total;
   static std::atomic<size_t> seconds_counter_ev;
   static std::atomic<size_t> seconds_counter;
   static std::atomic<size_t> frame_counter;
*/

   std::atomic<size_t> lines_counter;
   std::atomic<size_t> lines_counter_last;

   std::atomic<double> bytes_per_second;
   std::atomic<double> lines_per_second;
   std::atomic<size_t> file_size_total;
   std::atomic<size_t> seconds_counter_ev;
   std::atomic<size_t> seconds_counter;
   std::atomic<size_t> frame_counter;

   std::vector <std::string> v_buffer;


  CLogRotator *logrotator;
  CParameters *params;

  size_t log_current_size; //in bytes
  std::ofstream file_out;

  //char bf[65536];

  bool file_out_error;
  bool no_free_space;
  size_t test_string_size;
  std::string fname_template;


  CGenCycle *cycle;


  void server_handle();

  void write_results();
  void write (const std::string &s, bool rated);
  void write_buffered (const std::string &s, bool rated);

  void flush_buffer();

  bool open_logfile();

   CProducer (CParameters *prms, const std::string &fname);
   ~CProducer();
   void run();

};




#endif
