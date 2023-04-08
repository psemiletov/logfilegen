#ifndef CYCLE_H
#define CYCLE_H

#include <fstream>
#include <future>


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

  size_t frame_counter = 0;

  CGenCycle (CProducer *prod, CParameters *prms, const std::string &fname);

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
  std::string response;

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

  bool file_out_error;
  bool no_free_space;
  size_t test_string_size;
  std::string fname_template;

  CGenCycle *cycle;

  CProducer (CParameters *prms, const std::string &fname);
  ~CProducer();

  void run();
  bool open_logfile();

  void write (const std::string &s, bool rated);
  void write_buffered (const std::string &s, bool rated);
  void flush_buffer();

  void write_results();

  void server_init();
  void server_done();
  void server_handle();

};



#endif
