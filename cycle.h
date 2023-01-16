#ifndef CYCLE_H
#define CYCLE_H


#include <cstdint>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <fstream>
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

  bool use_gzip;

  size_t max_log_files;
  string max_log_file_size;

  bool pure;
  bool bstdout;
  bool debug;

  void print();
};


class CGenCycle
{
public:

  CParameters *params;
  CTpl *tpl;
  CLogRotator *logrotator;

  size_t log_current_size; //in bytes
  ofstream file_out;
  bool file_out_error;
  bool no_free_space;
  size_t test_string_size;
  string fname_template;

  CGenCycle (CParameters *prms, const string &fname);
  bool open_logfile();

  ~CGenCycle();
  virtual void loop() = 0;


};



class CGenCycleUnrated: public CGenCycle
{
public:

  CGenCycleUnrated (CParameters *prms, const string &fname);

  void loop();

};


#endif
