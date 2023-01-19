/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#ifndef UTL_H
#define UTL_H

//#include <string>
//#include <vector>
//#include <map>

using namespace std;

//#include <sys/statvfs.h>
#include <sys/stat.h>

#include <algorithm>
#include <filesystem>


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


bool is_path_abs (const string &path)
{
  if (path[0] == '/')
     return true;

 if (path[1] == ':') //windows
     return true;


  return false;
}

size_t get_file_size (const string &fname)
{
  struct stat buf;
  stat (fname.c_str(), &buf);
  return buf.st_size;
}


string get_file_ext (const string &fname)
{
  std::string::size_type i = fname.rfind('.');
  if (i != std::string::npos)
     return fname.substr (i+1);
  else
      return string();
}


string replace_file_ext (const string &fname, const string &ext)
{
  string result = fname;
  string::size_type i = fname.rfind ('.', fname.length());
  if (i != string::npos)
      result.replace (i+1, ext.length(), ext);

  return result;
}


string increase_file_ext (const string &fname)
{
  string ext = get_file_ext (fname);
  int i = atoi (ext.c_str());
  i++;
  ext = std::to_string(i);
  return replace_file_ext (fname, ext);
}


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

/*
size_t get_free_space (const string &path)
{


  struct statvfs buf;

  int r = statvfs (path.c_str(), &buf);

  if (r < 0)
     return -1;

  return buf.f_bavail * buf.f_bsize;
}
*/


size_t get_free_space (const string &path)
{
  if (path.empty())
     return 0;

//  cout << "get_free_space:" <<  path << endl;

  std::filesystem::path p (path);
  const std::filesystem::space_info i = std::filesystem::space (p);
  return i.available;
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



size_t string_to_file_size (const string &val)
{

  size_t result = 0;
/*

  const char *st = val.c_str();
  if (st)
     result = atoi (st);
*/
  char* end;

   const char *st = val.c_str();
   if (st)
     result = strtoull (st, &end, 10);


  string s = val;

  std::for_each (
                 s.begin(),
                 s.end(),
                 [](char & c) {
                               c = ::tolower(c);
                              });

  if (s.find ("k") != string::npos)
     result = result * 1024;

  if (s.find ("m") != string::npos)
     result = result * 1048576;

  if (s.find ("g") != string::npos)
     result = result * 1073741824;

  return result;
}

unsigned long long string_to_file_size_ull (const string &val)
{

  char* end;
     // finding the unsigned long
    // integer with base 36

  unsigned long long result = 0;
  const char *st = val.c_str();
  if (st)
     result = strtoull (st, &end, 10);


  string s = val;

  std::for_each (
                 s.begin(),
                 s.end(),
                 [](char & c) {
                               c = ::tolower(c);
                              });

  if (s.find ("k") != string::npos)
     result = result * 1024;

  if (s.find ("m") != string::npos)
     result = result * 1048576;

  if (s.find ("g") != string::npos)
     result = result * 1073741824;

  return result;
}




bool is_program_exists (const string &appname)
{
  string cm = "which " + appname + " > /dev/null 2>&1";
  if (system (cm.c_str()))
     return false;
  else
      return true;
}


string string_file_load (const string &fname)
{
 std::ifstream t (fname.c_str());
 std::string s ((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());

 return s;
}


string string_replace_all (const string &s, const string &from, const string &to)
{
  string result = s;
  size_t i = 0;
  do
    {
     i = result.find (from);
     if (i != string::npos)
         result = result.replace (i, from.length(), to);
    }
  while (i != string::npos);

  return result;
}

#endif
