/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#ifndef UTL_H
#define UTL_H

//#include <string>
//#include <vector>
//#include <map>

using namespace std;

#include <sys/statvfs.h>
#include <sys/stat.h>

#include <algorithm>

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>  // for FILENAME_MAX



#include "utl.h"


#ifdef WINDOWS
#include <direct.h>
#define get_cur_dir _getcwd
#else
#include <unistd.h>
#define get_cur_dir getcwd
#endif



#ifdef WINDOWS
#define DIR_SEPARATOR '\\'
#else
#define DIR_SEPARATOR '/'
#endif



string get_file_path (const string &path)
{
  char sep = '/';

#ifdef _WIN32
   sep = '\\';
#endif

  size_t i = path.rfind (sep, path.length());

  if (i != string::npos)
     return path.substr(0, i);

  return("");
}


string get_home_dir()
{
  string homedir = getenv ("HOME");
  return homedir;
}


string current_path()
{
  char path [FILENAME_MAX];
  string result;

  if (! get_cur_dir (path, sizeof (path)))
     return result;

  result = path;
  return result;
}


size_t get_free_space (const string &path)
{
  struct statvfs buf;

  int r = statvfs (path.c_str(), &buf);

  if (r < 0)
     return -1;

  return buf.f_bavail * buf.f_bsize;
}


bool file_exists (const string &name)
{
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}



string str_replace (string &source, const string &text_to_find, const string &replace_with)
{
  if (source.empty())
      return source;

  if (text_to_find.empty())
      return source;

  if (replace_with.empty())
      return source;

  size_t pos = source.find (text_to_find);

  if (pos == string::npos)
     return source;

  source.replace (pos, text_to_find.size(), replace_with);

  return source;
}


vector <string> split_string_to_vector (const string& s, const string& delimeter, const bool keep_empty = true)
{
  vector <string> result;

  if (delimeter.empty())
     {
      result.push_back(s);
      return result;
     }

  string::const_iterator substart = s.begin(), subend;

  while (true)
       {
        subend = search (substart, s.end(), delimeter.begin(), delimeter.end());

        string temp (substart, subend);

        if (keep_empty || ! temp.empty())
            result.push_back (temp);

        if (subend == s.end())
            break;

        substart = subend + delimeter.size();
       }

  return result;
}



#endif
