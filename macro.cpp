#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <algorithm>

#include "utl.h"
#include "macro.h"


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
  rnd_length = 0;
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

  vector <string> vt = split_string_to_vector (s, ":");

  if (vt.size() == 2)
      m->rnd_length = atoi (vt[1].c_str());

  if (vt.size() == 3)
      {
       m->len_min = atoi (vt[1].c_str());
       m->len_max = atoi (vt[2].c_str());
      }


  return m;
}


void CMacroStrRandom::parse (const string &s)
{

  vector <string> vt = split_string_to_vector (s, ":");

  if (vt.size() == 2)
      rnd_length = atoi (vt[1].c_str());

  if (vt.size() == 3)
      {
       len_min = atoi (vt[1].c_str());
       len_max = atoi (vt[2].c_str());
      }
}




string CMacroStrRandom::process()
{

  if (len_max == 0)
     return gen_string (rnd_generator, rnd_length);

   return gen_string (rnd_generator, len_min, len_max);
}


/*
string CMacroStrRandom::gen_string (size_t len)
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


string CMacroStrRandom::gen_string (size_t min, size_t max)
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
*/


CMacroIntRandom* CMacroIntRandom::create_self (const string &s)
{

  CMacroIntRandom *m = new CMacroIntRandom();

  vector <string> vt = split_string_to_vector (s, ":");

  if (vt.size() == 2)
      m->rnd_length = atoi (vt[1].c_str());

  if (vt.size() == 3)
      {
       m->len_min = atoi (vt[1].c_str());
       m->len_max = atoi (vt[2].c_str());
      }


  return m;
}


void CMacroIntRandom::parse (const string &s)
{

  vector <string> vt = split_string_to_vector (s, ":");

  if (vt.size() == 2)
      rnd_length = atoi (vt[1].c_str());

  if (vt.size() == 3)
      {
       len_min = atoi (vt[1].c_str());
       len_max = atoi (vt[2].c_str());
      }
}


string CMacroIntRandom::process()
{

  if (len_max == 0)
     return gen_number (rnd_generator, rnd_length);

   return gen_number (rnd_generator, len_min, len_max);
}
