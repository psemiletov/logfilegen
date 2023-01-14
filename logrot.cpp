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

  for (size_t i = 0; i < max_log_files; i++)
      {
       string f = source_filename + "." + std::to_string(i);
       filenames.push_back (f);
//       cout << "f: " << f << endl;
      }

}


void CLogRotator::rotate()
{
//  cout << "CLogRotator::rotate()" << endl;

  for (size_t i = filenames.size() - 1; i > 0; i--)
      {
       string oldname = filenames[i-1];
       string newname = filenames[i];
//        cout << "rename: " << oldname << " to: " << newname << endl;
        rename (oldname.c_str(), newname.c_str());
      }

  string fname = source_filename + ".0";
  rename (source_filename.c_str(), fname.c_str());

}


