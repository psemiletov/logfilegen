#include <iostream>

#include "logrot.h"
#include "utl.h"

CLogRotator::CLogRotator (const string &fname, size_t maxfiles, size_t maxfilesize)
{
  //cout << "CLogRotator::CLogRotator" << endl;
  source_filename = fname;
  max_log_files = maxfiles;
  max_log_file_size = maxfilesize;

 has_gzip = is_program_exists ("gzip");


  //cout << "source_filename: " << source_filename << endl;
//  cout << "max_log_files: " << max_log_files << endl;
  //cout << "max_log_file_size: " << max_log_file_size << endl;


  for (size_t i = 0; i < max_log_files; i++)
      {
       string f = source_filename + "." + std::to_string(i);
       filenames.push_back (f);
//       cout << "f: " << f << endl;
      }


}

/*

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

  if (has_gzip && use_gzip)
     {
    //  cout << "ZIP" << endl;
      string cm = "gzip " + source_filename;
      if (system (cm.c_str()) == 0)
         {
          string fnamezip = source_filename + ".gz";
          string fname = source_filename + ".0";

      //    cout << "fnamezip" << fnamezip << endl;
        //  cout << "fname" << fname << endl;

          rename (fnamezip.c_str(), fname.c_str());
         }
      else
          {//fallback if not zipped
           string fname = source_filename + ".0";
           rename (source_filename.c_str(), fname.c_str());
          }
     }
  else
      {
       string fname = source_filename + ".0";
       rename (source_filename.c_str(), fname.c_str());
     }


}

*/



void CLogRotator::rotate()
{
//  cout << "CLogRotator::rotate()" << endl;

  if (! use_gzip)
     {
      for (size_t i = filenames.size() - 1; i > 0; i--)
          {
           string oldname = filenames[i-1];
           string newname = filenames[i];
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

