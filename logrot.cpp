#include <iostream>

#include "logrot.h"
#include "utl.h"


using namespace std;

CLogRotator::CLogRotator (const std::string &fname, size_t maxfiles, size_t maxfilesize)
{
  source_filename = fname;
  max_log_files = maxfiles;
  max_log_file_size = maxfilesize;
  has_gzip = is_program_exists ("gzip");

  for (size_t i = 0; i < max_log_files; i++)
      {
       std::string f = source_filename + "." + std::to_string(i);
       filenames.push_back (f);
      }
}


void CLogRotator::rotate()
{
  if (! use_gzip)
     {
      for (size_t i = filenames.size() - 1; i > 0; i--)
          {
           std::string oldname = filenames[i-1];
           std::string newname = filenames[i];
//        cout << "rename: " << oldname << " to: " << newname << endl;
           rename (oldname.c_str(), newname.c_str());
          }

      string fname = source_filename + ".0";
      rename (source_filename.c_str(), fname.c_str());
      return;
     }

  if (has_gzip && use_gzip)
     {
      for (size_t i = filenames.size() - 1; i > 0; i--)
          {
           string oldname = filenames[i-1] + ".gz";
           string newname = filenames[i] + ".gz";
     //      cout << "rename: " << oldname << " to: " << newname << endl;
           rename (oldname.c_str(), newname.c_str());
          }

      string fname = source_filename + ".0";
      rename (source_filename.c_str(), fname.c_str());

       //  cout << "ZIP" << endl;
      string cm = "gzip " + fname;
      if (system (cm.c_str()) == 0)
         {
       //   cout << "ok " << fname << endl;
          ; //ok
         }
     }
}
