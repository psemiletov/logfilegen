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
#include "vars.h"

using namespace std;


std::mt19937 &vmt()
{
  // initialize once per thread
  //thread_local static std::random_device srd;
  //thread_local static std::mt19937 smt(srd());


  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  thread_local static std::mt19937 smt(seed);

  return smt;
}



int get_value_nature (const string &s)
{
  if (s.empty())
     return 0;

  if (s.find ("|") != string::npos)
     return VT_SEQ;

  if (s.find ("..") != string::npos)
     return VT_RANGE;

  return VT_SINGLE;
}


CVar::CVar (const string &key, const string &val)
{
  k = key;
  string value = val;

//  cout << "CVar::CVar key:" << key << " val:" << val << endl;

  rnd_length = 8;
  precision = 3;

//  rnd_generator = new std::mt19937 (rnd_dev());

  //rnd_generator = new std::mt19937 (std::chrono::system_clock::now().time_since_epoch().count());

  vartype = get_value_nature (val);

  //если макрос и не SEQ
  if (value[0] == '@' && vartype == VT_SINGLE)
     {
      macroname = get_macro_name (value);

      if (! macroname.empty())
         {
          auto f = pool.macros.find (macroname);
          if (f != pool.macros.end())
             f->second->parse (value);
         }
      }


  if (vartype == VT_SINGLE || vartype == VT_DATETIME)
     v.push_back (value);

  if (vartype == VT_SEQ)
     {
      v = split_string_to_vector (value, "|");

      for (size_t i = 0; i < v.size(); i++)
         {
          string t = v[i];

          if (t[0] != '@')
             continue;

          string name = get_macro_name (t);
          if (name.empty())
             continue;

          auto f = pool.macros.find (name);
          if (f == pool.macros.end())
             continue;

          CMacro *tm = f->second->create_self (t);

          cache.add (i, tm);
         }
     }

  if (vartype == VT_RANGE)
     {
      v = split_string_to_vector (value, "..");

       //check for int or fractional
      size_t pos = v[0].find (".");

      if (pos != string::npos)
         {
          vartype = VT_FLOATRANGE;
          fa = atof (v[0].c_str());
          fb = atof (v[1].c_str());
          precision = v[0].size() - pos - 1;
         }
       else
           {
            a = atoi (v[0].c_str());
            b = atoi (v[1].c_str());
           }
     }
}


CVar::~CVar()
{
  //delete rnd_generator;
}


int CVar::get_rnd (int ta, int tb)
{
   std::uniform_int_distribution <> distrib (ta, tb);
   return distrib (vmt());
}


string CVar::gen_msecs()
{
  std::uniform_real_distribution <> distrib (fa, fb);

//  std::string s;
  std::stringstream sstream;
  sstream.setf (std::ios::fixed);
  sstream.precision (precision);
  sstream << distrib (vmt());

  return sstream.str();
}


string CVar::get_val()
{
  string result;

  if (vartype == VT_SINGLE)
     result = v[0];
  else
  if (vartype == VT_RANGE)
     result = std::to_string (get_rnd (a, b));
  else
  if (vartype == VT_SEQ)
     {
      int i = get_rnd (0, v.size()-1);
      result = v[i];

      if (result[0] == '@')
        {
         auto f = cache.macros.find (i);
         if (f != cache.macros.end())
             result = f->second->process();
        }

     }
  else
  if (vartype == VT_FLOATRANGE)
     result = gen_msecs();

   //pre process macros

   //Если VT_SEQ и result еще не что-либо а макрос, инициализуем (парсим) его

  //иначе просто макрос, который был инициализиван в CVar
  if (! macroname.empty() && vartype == VT_SINGLE)
     {
      auto f = pool.macros.find (macroname);
      if (f != pool.macros.end())
         result = f->second->process();
      }

  return result;
}
