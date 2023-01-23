#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <algorithm>

#include "utl.h"
#include "macro.h"


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
//  cout << "CMacroStrRandom::create_self: " << s << endl;

  CMacroStrRandom *m = new CMacroStrRandom();

  vector <string> vt = split_string_to_vector (s, ":");

  if (vt.size() == 2)
      m->rnd_length = atoi (vt[1].c_str());

  if (vt.size() == 3)
      {
       m->len_min = atoi (vt[1].c_str());
       m->len_max = atoi (vt[2].c_str());
      }

  //cout << "rnd_length: " << m->rnd_length << endl;
//
//  cout << "len_min: " << m->len_min << endl;

//  cout << "len_max: " << m->len_max << endl;

  //  cout << "CMacroStrRandom::create_self END: " << s << endl;


 // m->test = "!!!! test";

 // m->rnd_length = 13;

  return m;
}


string CMacroStrRandom::process()
{
  //cout << "CMacroStrRandom::process()" << endl;

  //cout << "rnd_length: " << rnd_length << endl;

  //cout << "len_min: " << len_min << endl;

//  cout << "len_max: " << len_max << endl;


 // cout << "test: " << test << endl;

  if (len_max == 0)
     return gen_string (rnd_length);

   return gen_string (len_min, len_max);
}



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
