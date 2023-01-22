#ifndef PARAMS_H
#define PARAMS_H

#include <string>

using namespace std;


class CParameters
{
public:

  string templatefile; //templatefile name from /logfilegen/templates
  string logfile; //output logfile name with full path
  string mode;    //"nginx" or "apache"

  int duration; //duration of log generation, in seconds
  int rate;  //during the log generation, how many lines per second will be written
  size_t lines;
  size_t size;
  string s_size;

  size_t max_log_files;
  string max_log_file_size;

  string timestamp;
  bool random;

  bool use_gzip;
  bool pure;
  bool bstdout;
  bool debug;
  bool benchmark;
  bool stats;


  void print();
};

#endif
