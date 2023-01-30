#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <vector>

#include "utl.h"
#include "macro.h"


char arr_nums [] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};


int get_rnd (std::mt19937 *rnd_generator, int ta, int tb)
{
  std::uniform_int_distribution <> distrib (ta, tb);
  return distrib (*rnd_generator);
}


string gen_string (std::mt19937 *rnd_generator, size_t len)
{
  ostringstream st;

  std::uniform_int_distribution<> distrib (0, 25);

  for (size_t i = 0; i < len; i++)
      {
       int g = distrib (*rnd_generator);
       char d = static_cast<char> (g + 'a');
       st << d;
      }

  return st.str();
}


string gen_string (std::mt19937 *rnd_generator, size_t min, size_t max)
{
  ostringstream st;

  std::uniform_int_distribution<> distrib (0, 25);
  std::uniform_int_distribution<> dminmax (min, max);
  size_t len = dminmax (*rnd_generator);

  for (size_t i = 0; i < len; i++)
      {
       int g = distrib (*rnd_generator);
       char d = static_cast<char> (g + 'a');
       st << d;
      }

  return st.str();
}


string gen_rnd_path (std::mt19937 *rnd_generator, size_t min, size_t max, size_t deep)
{
  string result;

  size_t deep_max = get_rnd (rnd_generator, 1, deep);

  for (size_t d = 0; d < deep_max; d++)
      {
       result += "/";
       int len = get_rnd (rnd_generator, min, max);

       result += gen_string (rnd_generator, len);
      }

  return result;
}


string gen_number (std::mt19937 *rnd_generator, size_t len)
{
  std::uniform_int_distribution<> distrib (0, 9);

  string result;

  for (size_t i = 0; i < len; i++)
      {
       result += arr_nums[distrib (*rnd_generator)];
      }

  return result;
}


string gen_number (std::mt19937 *rnd_generator, size_t min, size_t max)
{
  std::uniform_int_distribution<> distrib (0, 9);
  std::uniform_int_distribution<> dminmax (min, max);
  size_t len = dminmax (*rnd_generator);

  string result;

  for (size_t i = 0; i < len; i++)
      {
       result += arr_nums[distrib (*rnd_generator)];
      }

  return result;
}


string gen_hex_number (std::mt19937 *rnd_generator, size_t len)
{
  std::uniform_int_distribution<> distrib (0, 15);

  string result;

  for (size_t i = 0; i < len; i++)
      {
       result += arr_nums[distrib (*rnd_generator)];
      }

  return result;
}


string gen_hex_number (std::mt19937 *rnd_generator, size_t min, size_t max)
{
  std::uniform_int_distribution<> distrib (0, 15);
  std::uniform_int_distribution<> dminmax (min, max);
  size_t len = dminmax (*rnd_generator);

  string result;

  for (size_t i = 0; i < len; i++)
      {
       result += arr_nums[distrib (*rnd_generator)];
      }

  return result;
}


CMacro::CMacro()
{
  rnd_generator = new std::mt19937 (rnd_dev());

  len_min = 0;
  len_max = 0;
  length = 0;
}


CMacro::~CMacro()
{
  delete rnd_generator;
}


CMacrosPool::CMacrosPool()
{
   macros.insert (std::make_pair ("@ip", new CMacroIPRandom()));
   macros.insert (std::make_pair ("@str", new CMacroStrRandom()));
   macros.insert (std::make_pair ("@int", new CMacroIntRandom()));
   macros.insert (std::make_pair ("@hex", new CMacroHexRandom()));
   macros.insert (std::make_pair ("@datetime", new CMacroDateTime()));
   macros.insert (std::make_pair ("@path", new CMacroPathRandom()));
   macros.insert (std::make_pair ("@file", new CMacroFileSource()));
}


CMacrosPool::~CMacrosPool()
{
  for (auto itr = macros.begin(); itr != macros.end(); ++itr)
      {
       delete (itr->second);
      }
}



CMacroIPRandom* CMacroIPRandom::create_self (const string &s)
{
  return new CMacroIPRandom();

}



string CMacroIPRandom::process()
{
  std::uniform_int_distribution<> distrib (0, 255);

  string result;

  result += to_string (distrib (*rnd_generator));
  result += ".";

  result += to_string (distrib (*rnd_generator));
  result += ".";

  result += to_string (distrib (*rnd_generator));
  result += ".";

  result += to_string (distrib (*rnd_generator));

  return result;
}


CMacroStrRandom* CMacroStrRandom::create_self (const string &s)
{
  CMacroStrRandom *m = new CMacroStrRandom();
  m->parse (s);
  return m;
}


void CMacroStrRandom::parse (const string &s)
{
  len_min = 0;
  len_max = 0;
  length = 0;

  vector <string> vt = split_string_to_vector (s, ":");

  if (vt.size() == 2)
      length = atoi (vt[1].c_str());

  if (vt.size() == 3)
      {
       len_min = atoi (vt[1].c_str());
       len_max = atoi (vt[2].c_str());
      }
}


string CMacroStrRandom::process()
{
  if (len_max == 0)
     return gen_string (rnd_generator, length);

   return gen_string (rnd_generator, len_min, len_max);
}


CMacroIntRandom* CMacroIntRandom::create_self (const string &s)
{
  CMacroIntRandom *m = new CMacroIntRandom();
  m->parse (s);
  return m;
}


void CMacroIntRandom::parse (const string &s)
{
  len_min = 0;
  len_max = 0;
  length = 0;

  vector <string> vt = split_string_to_vector (s, ":");

  if (vt.size() == 2)
      length = atoi (vt[1].c_str());

  if (vt.size() == 3)
      {
       len_min = atoi (vt[1].c_str());
       len_max = atoi (vt[2].c_str());
      }
}


string CMacroIntRandom::process()
{
  if (len_max == 0)
     return gen_number (rnd_generator, length);

   return gen_number (rnd_generator, len_min, len_max);
}



CMacroHexRandom* CMacroHexRandom::create_self (const string &s)
{
  CMacroHexRandom *m = new CMacroHexRandom();
  m->parse (s);
  return m;
}


void CMacroHexRandom::parse (const string &s)
{
  len_min = 0;
  len_max = 0;
  length = 0;

  vector <string> vt = split_string_to_vector (s, ":");

  if (vt.size() == 2)
      length = atoi (vt[1].c_str());

  if (vt.size() == 3)
      {
       len_min = atoi (vt[1].c_str());
       len_max = atoi (vt[2].c_str());
      }
}


string CMacroHexRandom::process()
{
  if (len_max == 0)
     return gen_hex_number (rnd_generator, length);

   return gen_hex_number (rnd_generator, len_min, len_max);
}



CMacroDateTime* CMacroDateTime::create_self (const string &s)
{
  CMacroDateTime *m = new CMacroDateTime();
  m->parse (s);
  return m;
}


void CMacroDateTime::parse (const string &s)
{
  len_min = 0;
  len_max = 0;
  length = 0;
  text = "";

  size_t pos = s.find (":");
  if (pos != string::npos)
     text = s.substr (pos + 1);

}

/*  SLOW
string CMacroDateTime::process()
{
  if (text.empty())
     return string();

  auto t = std::time (nullptr);
  auto tm = *std::localtime(&t);

  std::ostringstream oss;
  oss << std::put_time (&tm, text.c_str());

  auto result = oss.str();
  return result;
}
*/



string CMacroDateTime::process()
{
  if (text.empty())
     return string();

  time_t rawtime;
  struct tm * timeinfo;

  time (&rawtime);
  timeinfo = localtime (&rawtime);

  char buffer [128];
  strftime (buffer, 128, text.c_str(), timeinfo);
  //std::ostringstream oss;
  //oss << std::put_time (&tm, text.c_str());

  auto result = buffer;
  return result;
}




CMacroPathRandom* CMacroPathRandom::create_self (const string &s)
{
  CMacroPathRandom *m = new CMacroPathRandom();
  m->parse (s);
  return m;
}


void CMacroPathRandom::parse (const string &s)
{

  len_min = 0;
  len_max = 0;
  length = 0;
  text = "";

  vector <string> vt = split_string_to_vector (s, ":");
  if (vt.size() == 4)
     {
     len_min = atoi (vt[1].c_str());
     len_max = atoi (vt[2].c_str());
     length = atoi (vt[3].c_str());

     if (length == 0)
         length = 1;
    }

}


string CMacroPathRandom::process()
{
   return gen_rnd_path (rnd_generator, len_min, len_max, length);
}


CMacroFileSource* CMacroFileSource::create_self (const string &s)
{
  CMacroFileSource *m = new CMacroFileSource();
  m->parse (s);
  return m;
}



void CMacroFileSource::parse (const string &s)
{

  //cout << "void CMacroFileSource::parse (const string &s) " << s << endl;

  len_min = 0;
  len_max = 0;
  length = 0;
  text = "";

  size_t pos = s.find (":");
  if (pos == string::npos)
      return;

  string path = s.substr (pos + 1);

  //cout << "path " << path << endl;

  vt = vector_file_load (path);

  //cout << "vt.size() " << vt.size() << endl;


}



/*
void CMacroFileSource::parse (const string &s)
{

  len_min = 0;
  len_max = 0;
  length = 0;
  text = "";

  size_t pos = s.find (":");
  if (pos == string::npos)
      return;

  string path = s.substr (pos + 1);
  string t1 = string_file_load (path);

  if (! t1.empty())
     {
      string t2 = string_replace_all (t1, "\n", "|");
      t2 = string_replace_all (t2, "\r\n", "|"); //for windows
      t2.pop_back(); //remove last |

      vector <string> v; //values
      v = split_string_to_vector (t2, "|");

      //vector <string> v = vector_file_load (path);
      text = v[get_rnd (rnd_generator, 0, v.size()-1)];
     }
}
*/

string CMacroFileSource::process()
{
  //cout << "CMacroFileSource::process()" << endl;

  if (vt.size() != 0)
     text = vt[get_rnd (rnd_generator, 0, vt.size()-1)];

  return text;
}



void CMacrosCache::add (size_t pos, CMacro *m)
{
   macros.insert (std::make_pair (pos, m));
}


CMacrosCache::~CMacrosCache()
{
  for (auto itr = macros.begin(); itr != macros.end(); ++itr)
      {
       delete (itr->second);
      }
}
