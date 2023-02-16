#ifndef PARAMS_H
#define PARAMS_H

#include <string>

//using namespace std;


class CParameters
{
public:

  std::string templatefile; //templatefile name from /logfilegen/templates
  std::string logfile; //output logfile name with full path
  std::string mode;    //"nginx" or "apache"

  size_t duration; //duration of log generation, in seconds
  size_t rate;  //during the log generation, how many lines per second will be written
  size_t lines;
  size_t size;
  std::string s_size;

  std::string metrics_addr;
  std::string port;
  std::string ip; //public ip

  size_t max_log_files;
  std::string max_log_file_size;

  std::string timestamp;
  bool random;

  bool use_gzip;
  bool pure;
  bool bstdout;
  bool debug;
  bool benchmark;
  bool stats;
  bool test;
  bool metrics;
  size_t poll;

  void print();
};

#endif
