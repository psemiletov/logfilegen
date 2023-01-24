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
/*cout << "int get_rnd (std::mt19937 *rnd_generator, int ta, int tb)" << endl;
cout << "int ta " << ta << endl;
cout << "int bb " << tb << endl;


*/
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
 /* cout << "string gen_rnd_path " << endl;
  cout << "deep " << deep << endl;
  cout << "min " << min << endl;
  cout << "max " << max << endl;
*/
  string result;

  size_t deep_max = get_rnd (rnd_generator, 1, deep);

  cout << "2" << endl;

  for (size_t d = 0; d < deep_max; d++)
      {
       result += "/";
       int len = get_rnd (rnd_generator, min, max);

       cout << "len:" << len << endl;

       result += gen_string (rnd_generator, len);
      }

  cout << "string gen_rnd_path 2" << endl;


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
   macros.insert (std::make_pair ("@ip_random", new CMacroIPRandom()));
   macros.insert (std::make_pair ("@str_random", new CMacroStrRandom()));
   macros.insert (std::make_pair ("@int_random", new CMacroIntRandom()));
   macros.insert (std::make_pair ("@datetime", new CMacroDateTime()));
   macros.insert (std::make_pair ("@str_path", new CPathRandom()));

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



CPathRandom* CPathRandom::create_self (const string &s)
{

  CPathRandom *m = new CPathRandom();
  m->parse (s);

  return m;
}


void CPathRandom::parse (const string &s)
{

//cout << " void CPathRandom::parse (const string &s): " << s << endl;


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
/*

cout << "void CPathRandom::parse (const string &s) --- 1" << endl;

cout << "len_min: " << len_min << endl;
cout << "len_max: " << len_max << endl;
cout << "length: " << length << endl;


cout << "void CPathRandom::parse (const string &s) --- 2" << endl;
*/
    }

}


string CPathRandom::process()
{
  cout << "CPathRandom::process()" << endl;

  return gen_rnd_path (rnd_generator, len_min, len_max, length);
}
