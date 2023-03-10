#ifndef CYCLE_H
#define CYCLE_H

#include <fstream>

#ifndef PROM
#include <future>
#endif


#ifndef PROM

#if defined(_WIN32) || defined(_WIN64)
//#include <WinSock2.h>
//#include <WinSock.h>
#include <winsock2.h>
#endif

#ifdef __HAIKU__
#include <Socket.h>
#endif


#ifdef __unix__
#include <netinet/in.h>
#include <sys/socket.h>
//#include <arpa/inet.h>
#endif

#endif



#ifdef PROM

#include "prometheus/client_metric.h"
#include "prometheus/counter.h"
#include "prometheus/exposer.h"
#include "prometheus/family.h"
#include "prometheus/info.h"
#include "prometheus/registry.h"

using namespace prometheus;

#endif


#include "pairfile.h"
#include "utl.h"
#include "tpl.h"
#include "logrot.h"
#include "params.h"




class CGenCycle
{
public:

  CParameters *params;
  CTpl *tpl;
  CLogRotator *logrotator;

  size_t log_current_size; //in bytes
  std::ofstream file_out;

  bool file_out_error;
  bool no_free_space;
  size_t test_string_size;
  std::string fname_template;

  ///////// stats
  double bytes_per_second;
  double lines_per_second;
  size_t file_size_total;
  size_t lines_counter_last = 0;
  size_t lines_counter = 0;
  size_t seconds_counter_ev = 0;
  size_t seconds_counter = 0;
  size_t frame_counter = 0;
  //////

 ///SERV

#ifndef PROM

  int /*sockfd, */newsockfd, portno;

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

#endif

#ifdef PROM

  Exposer *exposer;
  std::shared_ptr< prometheus::Registry > registry;

#endif

  CGenCycle (CParameters *prms, const std::string &fname);

#ifndef PROM
  void server_handle();
#endif

  bool open_logfile();
  void write_results();

  ~CGenCycle();
  virtual void loop() = 0;

};


class CGenCycleRated: public CGenCycle
{
public:

  CGenCycleRated (CParameters *prms, const std::string &fname);
  void loop();

};


class CGenCycleUnrated: public CGenCycle
{
public:

  CGenCycleUnrated (CParameters *prms, const std::string &fname);
  void loop();

};


#endif
