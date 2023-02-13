#ifndef CYCLE_H
#define CYCLE_H

#include <cstdlib>
#include <fstream>
#include <csignal>


//SERV
#include <unistd.h>
#include <sys/types.h>
//#include <sys/socket.h>
#include <thread>
//


#if defined(_WIN32) || defined(_WIN64)

#undef _HAS_STD_BYTE

    #include <winsock2.h>
//    include <WinSock2.h>
//include <WinSock.h>
#else
   #include <netinet/in.h>
    #include <sys/socket.h>
//    #include <arpa/inet.h>
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



//using namespace std;


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

    double lines_per_second;
    size_t file_size_total;

   size_t lines_counter = 0;
   size_t seconds_counter_ev = 0;

   size_t seconds_counter = 0;
   size_t frame_counter = 0;
  //////


   ///SERV
     int sockfd, newsockfd, portno;


     #if defined(_WIN32) || defined(_WIN64)

      int clilen;

#else

      socklen_t clilen;

#endif



     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     bool server_run;
  //   std::future<void> f_handle;
     std::thread *th_srv;

    std::string response;



  #ifdef PROM

      Exposer *exposer;
     std::shared_ptr< prometheus::Registry > registry;
//prometheus::Family<prometheus::Counter> &countera;
#endif

  CGenCycle (CParameters *prms, const std::string &fname);

  void server_handle();

  bool open_logfile();

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
