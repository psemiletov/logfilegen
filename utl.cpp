/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/


#include <sys/stat.h>
#include <algorithm>

/*
#ifndef USE_OPENSUSE
#include <filesystem>
#else
#include <experimental/filesystem>
#endif
*/

/*
#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __cplusplus >= 201703L && __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  endif
#endif
*/


#include <cstdint>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>  // for FILENAME_MAX
#include <limits>
#include <unistd.h>

#if defined(_WIN32) || defined(_WIN64)

//#include <pwd.h>

#include <windows.h>
#include <Shlobj.h>


#define GetCurrentDir _getcwd
#define DIR_SEPARATOR '\\'

#else

#include <sys/statvfs.h>


#define DIR_SEPARATOR '/'

//#define GetCurrentDir getcwd


#endif


#include "utl.h"





using namespace std;


size_t get_file_size (const string &fname)
{
  if (fname.empty())
      return 0;

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


string get_file_path (const string &path)
{
  size_t i = path.rfind (DIR_SEPARATOR, path.length());

  if (i != string::npos)
     return path.substr (0, i);

  return string();
}


string get_home_dir()
{
  string result;

#if !defined(_WIN32) || !defined(_WIN64)

  const char *homedir = getenv ("HOME");

  if (homedir != NULL)
     result = homedir;
  /*else
      homedir = getpwuid(getuid())->pw_dir;

  if (homedir != NULL)
     result = homedir;
*/
//  return result;

#else

  char homeDirStr[MAX_PATH];

 if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, homeDirStr)))
   result = homeDirStr;


#endif

  return result;

}

/*
string current_path()
{
  return fs::current_path().string();
}
*/


string current_path()
{
 std::string result;

 char buff[FILENAME_MAX];

#if defined(_WIN32) || defined(_WIN64)

  if (_getcwd (buff, FILENAME_MAX))
    result = buff;


#else

 if (getcwd (buff, FILENAME_MAX))
    result = buff;

#endif

 return result;


}



string get_tmp_dir()
{
  std::string result;

#if defined(_WIN32) || defined(_WIN64)


  TCHAR szPath[MAX_PATH + 1];
  DWORD d = GetTempPath(MAX_PATH + 1, szPath);
  if (d != ERROR_SUCCESS)
     result = "/tmp";
   else
        result = szPath;

 #else


  char const *d = getenv ("TMPDIR");
  if (d)
     result = d;
  else
      result = "/tmp";

#endif

  return result;
}





/*
string current_path()
{
  char path [FILENAME_MAX];
  string result;

  if (! get_cur_dir (path, sizeof (path)))
     return result;

  result = path;
  return result;
}

*/

size_t get_free_space (const string &path)
{
  if (path.empty())
     return 0;

  size_t result = 0;

#if defined(_WIN32) || defined(_WIN64)


     BOOL  fResult;
      unsigned __int64 i64FreeBytesToCaller,
                       i64TotalBytes,
                       i64FreeBytes;
         fResult = GetDiskFreeSpaceEx (path.c_str(),
                                 (PULARGE_INTEGER)&i64FreeBytesToCaller,
                                 (PULARGE_INTEGER)&i64TotalBytes,
                                 (PULARGE_INTEGER)&i64FreeBytes);
         if (fResult)
         {
            result = i64FreeBytesToCaller;
            /*printf ("\n\nGetDiskFreeSpaceEx reports\n\n");
            printf ("Available space to caller = %I64u MB\n",
                    i64FreeBytesToCaller / (1024*1024));
            printf ("Total space               = %I64u MB\n",
                    i64TotalBytes / (1024*1024));
            printf ("Free space on drive       = %I64u MB\n",
                    i64FreeBytes / (1024*1024));*/
         }



#else

  struct statvfs buf;

  int r = statvfs (path.c_str(), &buf);

  if (r < 0)
     result = 0;
  else
  result = buf.f_bavail * buf.f_bsize;

#endif

  return result;

}


/*
size_t get_free_space (const string &path)
{
  if (path.empty())
     return 0;

  fs::path p (path);
  const fs::space_info i = fs::space (p);
  return i.available;
}
*/

bool file_exists (const string &name)
{
  if (name.empty())
     return false;

  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}


bool is_program_exists (const string &appname)
{
  if (appname.empty())
     return false;

  string cm = "which " + appname + " > /dev/null 2>&1";
  if (system (cm.c_str()))
     return false;
  else
      return true;
}


bool is_path_abs (const string &path)
{
  if (path.empty())
     return false;

  if (path[0] == '/')
     return true;

  if (path.size() > 1)
     if (path[1] == ':') //windows
        return true;

  return false;
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


string string_replace_all (const string &s, const string &from, const string &to)
{
  string result = s;
  size_t i = 0;
  do
    {
     i = result.find (from);
     if (i != string::npos)
         result.replace (i, from.length(), to);
    }
  while (i != string::npos);

  return result;
}


vector <string> split_string_to_vector (const string& s, const string& delimeter, const bool keep_empty)
{
  vector <string> result;

  if (delimeter.empty())
     {
      result.push_back (s);
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


string string_to_lower (const string &s)
{
  string result = s;

  std::for_each (
                 result.begin(),
                 result.end(),
                 [](char & c) {
                               c = ::tolower(c);
                              });

  return result;
}


size_t string_to_file_size (const string &val)
{
  size_t result = 0;

  char* end;

  const char *st = val.c_str();

  if (st)
     result = strtoull (st, &end, 10);

  string s = string_to_lower (val);

  if (s.find ("k") != string::npos)
     result = result * 1024;

  if (s.find ("m") != string::npos)
     result = result * 1048576;

  if (s.find ("g") != string::npos)
     result = result * 1073741824;

  return result;
}



string bytes_to_file_size (size_t val)
{
  if (val >= 1073741824)
     return std::to_string (val / 1073741824) + " gbytes";

  if (val >= 1048576)
     return std::to_string (val / 1048576) + " mbytes";

  if (val >= 1024)
     return std::to_string (val / 1024) + " kbytes";

  return std::to_string (val) + " bytes";;
}



string bytes_to_file_size3 (size_t val)
{
  if (val >= 1073741824)
     return format3 (std::to_string (val / 1073741824)) + " gbytes";

  if (val >= 1048576)
     return format3 (std::to_string (val / 1048576)) + " mbytes";

  if (val >= 1024)
     return format3 (std::to_string (val / 1024)) + " kbytes";

  return format3 (std::to_string (val)) + " bytes";;
}




string string_file_load (const string &fname)
{
 if (fname.empty())
    return string();

 std::ifstream t (fname.c_str());
 std::string s ((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());

 return s;
}


vector <string> vector_file_load (const string &fname)
{
  vector <string> v;

  if (fname.empty())
     return v;


  ifstream infile (fname.c_str());

  if (infile.fail())
     {
    //  cout << "CPairFile::CPairFile - Could not open file: " << fname << endl;
      return v;
     }

  string line;

  while (getline (infile, line))
        {
         v.push_back (line);
        }

   infile.close();
   return v;
}


string get_macro_name (const string &value)
{
 size_t pos = value.find_first_of (':');
 if (pos == string::npos)
     pos = value.size();

  return value.substr (0, pos);
}


void string_save_to_file (const string &fname, const string &s)
{
  std::ofstream file (fname);
  if (file.is_open())
  file << s;
  file.close();
}


bool path_exists (const string &fname)
{
  if (fname.empty())
     return false;

  std::string fpath = get_file_path (fname);
  if (fpath.empty())
     return false;

  return file_exists(fpath);
}



std::string format3 (size_t n)
{
  std::string number_str = std::to_string(n);

  for (int i = number_str.length() - 3; i > 0; i -= 3)
      number_str.insert(i, ",");

  return number_str;
}


std::string format3 (const std::string &s)
{
  std::string number_str = s;

  for (int i = number_str.length() - 3; i > 0; i -= 3)
      number_str.insert(i, ",");

  return number_str;
}
