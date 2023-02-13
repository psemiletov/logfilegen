/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#ifndef LOGROT_H
#define LOGROT_H

#include <string>
#include <vector>

//using namespace std;


class CLogRotator
{
public:

  bool has_gzip;
  bool use_gzip;

  size_t max_log_files;
  size_t max_log_file_size;

  std::string source_filename;
  std::vector <std::string> filenames;

  CLogRotator (const std::string &fname, size_t maxfiles, size_t maxfilesize);
  void rotate();

};

#endif
