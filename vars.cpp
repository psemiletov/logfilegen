//full nginx vars see at
//http://nginx.org/en/docs/varindex.html
//Apache:
//https://httpd.apache.org/docs/current/mod/mod_log_config.html

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


int get_value_nature (const string &s)
{
  if (s.empty())
     return 0;

  if (s.find ("|") != string::npos)
     return VT_SEQ;

  if (s.find ("..") != string::npos)
     return VT_RANGE;

//  if (s.find ("@") != string::npos)
  //   return VT_MACRO;


  return VT_SINGLE;
}


string get_macro_name (const string &value)
{
 size_t pos = value.find_first_of (':');
 if (pos == string::npos)
     pos = value.size();

  return value.substr (0, pos);
}


CVar::CVar (const string &key, const string &val)
{
  k = key;
  string value = val;


///
  rnd_length = 8;
  precision = 3;
  len_min = 0;
  len_max = 0;

  rnd_generator = new std::mt19937 (rnd_dev());
  vartype = get_value_nature (val);


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
  else
  if (vartype == VT_SEQ)
      v = split_string_to_vector (value, "|");
  else
  if (vartype == VT_RANGE)
     {
      v = split_string_to_vector (value, "..");

       //check int or float
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
  delete rnd_generator;

    for (auto itr = macros.begin(); itr != macros.end(); ++itr)
      {
       delete (itr->second);
      }

}



int CVar::get_rnd (int ta, int tb)
{
   std::uniform_int_distribution <> distrib (ta, tb);
   return distrib (*rnd_generator);
}


string CVar::gen_msecs()
{
  std::uniform_real_distribution <> distrib (fa, fb);

  std::string s;
  std::stringstream sstream;
  sstream.setf(std::ios::fixed);
  sstream.precision (precision);
  sstream << distrib (*rnd_generator);

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
      result = v[get_rnd (0, v.size()-1)];
  else
  if (vartype == VT_FLOATRANGE)
     result = gen_msecs();

   //pre process macros


   if (vartype == VT_SEQ && result[0] == '@')
       macroname = get_macro_name (result);

   //handle macros


  if (result[0] == '@')
     {
      macroname = get_macro_name (result);

      if (! macroname.empty())
         {
          auto f = pool.macros.find (macroname);
          if (f != pool.macros.end())
            f->second->parse (result);
         }
     }

  if (! macroname.empty())
     {
     auto f = pool.macros.find (macroname);
     if (f != pool.macros.end())
        result = f->second->process();
     }

  return result;
}