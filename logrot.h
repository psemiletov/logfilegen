/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#ifndef LOGROT_H
#define LOGROT_H

#include <string>
#include <vector>

using namespace std;


class CLogRotator
{
public:

  size_t max_log_files;
  size_t max_log_file_size;

  string source_filename;
  vector <string> filenames;

  CLogRotator (const string &fname, size_t maxfiles, size_t maxfilesize);
  void rotate();

};

#endif
