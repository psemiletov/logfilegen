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
#include "params.h"


using namespace std;
using namespace std::chrono;




class CGenCycle
{
public:

  CParameters *params;
  CTpl *tpl;
  CLogRotator *logrotator;

  size_t log_current_size; //in bytes
  ofstream file_out;
  /*unsigned long long*/ size_t file_size_total;

  bool file_out_error;
  bool no_free_space;
  //bool no_duration;
  size_t test_string_size;
  string fname_template;

  CGenCycle (CParameters *prms, const string &fname);
  bool open_logfile();

  ~CGenCycle();
  virtual void loop() = 0;

};


class CGenCycleRated: public CGenCycle
{
public:

  CGenCycleRated (CParameters *prms, const string &fname);
  void loop();

};


class CGenCycleUnrated: public CGenCycle
{
public:

  CGenCycleUnrated (CParameters *prms, const string &fname);
  void loop();

};


#endif
