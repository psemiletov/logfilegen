#include <iostream>

#include "logrot.h"
#include "utl.h"

CLogRotator::CLogRotator (const string &fname, size_t maxfiles, size_t maxfilesize)
{
  cout << "CLogRotator::CLogRotator" << endl;
  source_filename = fname;
  max_log_files = maxfiles;
  max_log_file_size = maxfilesize;


  cout << "source_filename: " << source_filename << endl;
  cout << "max_log_files: " << max_log_files << endl;
  cout << "max_log_file_size: " << max_log_file_size << endl;


}


void CLogRotator::rotate()
{
  cout << "CLogRotator::rotate()" << endl;


}

