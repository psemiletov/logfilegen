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
#include <sys/time.h>


#include "utl.h"
#include "macro.h"


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

       cout << "len:" << len << endl;

       result += gen_string (rnd_generator, len);
      }

  return result;
}


string gen_number (std::mt19937 *rnd_generator, size_t len)
{
  std::uniform_int_distribution<> distrib (0, 9);

  ostringstream st;

  for (size_t i = 0; i < len; i++)
      {
       st << distrib (*rnd_generator);
      }

  return st.str();
}


string gen_number (std::mt19937 *rnd_generator, size_t min, size_t max)
{
  std::uniform_int_distribution<> distrib (0, 9);
  std::uniform_int_distribution<> dminmax (min, max);
  size_t len = dminmax (*rnd_generator);

  ostringstream st;

  for (size_t i = 0; i < len; i++)
      {
       st << distrib (*rnd_generator);
      }

  return st.str();
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
   macros.push_back (new CMacroIPRandom());
   macros.push_back (new CMacroStrRandom());
   macros.push_back (new CMacroIntRandom());
   macros.push_back (new CMacroDateTime());
   macros.push_back (new CMacroPathRandom());
   macros.push_back (new CMacroFileSource());
}



CMacrosPool::~CMacrosPool()
{
  for (vector<CMacro*>::iterator it = macros.begin(); it != macros.end(); it++)
     {
       delete *it;
     }
}


CMacroIPRandom::CMacroIPRandom(): CMacro()
{
  name = "@ip_random";
}



CMacroIPRandom* CMacroIPRandom::create_self (const string &s)
{
  return new CMacroIPRandom();

}


string CMacroIPRandom::process()
{
  std::uniform_int_distribution<> distrib (0, 255);

  ostringstream st;

  st << distrib (*rnd_generator);
  st << ".";

  st << distrib (*rnd_generator);
  st << ".";

  st << distrib (*rnd_generator);
  st << ".";

  st << distrib (*rnd_generator);

  return st.str();
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



CMacroStrRandom::CMacroStrRandom(): CMacro()
{
  name = "@str_random";
}


string CMacroStrRandom::process()
{

  if (len_max == 0)
     return gen_string (rnd_generator, length);

   return gen_string (rnd_generator, len_min, len_max);
}



CMacroIntRandom::CMacroIntRandom(): CMacro()
{
  name = "@int_random";
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




CMacroDateTime::CMacroDateTime(): CMacro()
{
  name = "@datetime";
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




CMacroPathRandom::CMacroPathRandom(): CMacro()
{
  name = "@str_path";
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


CMacroFileSource::CMacroFileSource(): CMacro()
{
  name = "@file_source";
}


CMacroFileSource* CMacroFileSource::create_self (const string &s)
{
  CMacroFileSource *m = new CMacroFileSource();
  m->parse (s);
  return m;
}



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

  vt = vector_file_load (path);
}


string CMacroFileSource::process()
{

  if (vt.size() != 0)
     text = vt[get_rnd (rnd_generator, 0, vt.size()-1)];

  return text;
}



CMacro* CMacrosPool::find (const string &name)
{
  CMacro *m = NULL;

  for (size_t i = 0; i < macros.size(); i++)
     {
      if (macros[i]->name == name)
         {
          m = macros.at(i);
          break;
         }

     }

  return m;
}

