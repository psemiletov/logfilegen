#ifndef CYCLE_H
#define CYCLE_H


#include <cstdint>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <csignal>


#include "pairfile.h"
#include "utl.h"
#include "tpl.h"
#include "logrot.h"


using namespace std;
using namespace std::chrono;



class CParameters
{
public:

  string templatefile; //templatefile name from /logfilegen/templates
  string logfile; //output logfile name with full path
  string mode;    //"nginx" or "apache"

  int duration; //duration of log generation, in seconds
  int rate;  //during the log generation, how many lines per second will be written

  size_t max_log_files;
  string max_log_file_size;

  bool pure;
  bool bstdout;
  bool debug;

  void print();
};




class CGenCycleUnrated
{
public:

  CParameters *params;
  CTpl *tpl;

  CLogRotator *logrotator;

  ofstream file_out;
  bool file_out_error;

  string fname_template;


  CGenCycleUnrated (CParameters *prms, const string &fname);
  ~CGenCycleUnrated();

  bool init();
  void loop();


};


#endif
