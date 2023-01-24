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

  if (s.find ("@") != string::npos)
     return VT_MACRO;


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

////

  if (value[0] == '@')
  {
     macroname = get_macro_name (value);

     if (! macroname.empty())
  {
   auto f = pool.macros.find (macroname);
   if (f != pool.macros.end())
      {
       //cout << "f->second->len_min:" << f->second->len_min  << endl;
       f->second->parse (value);
//      result = f->first;

     }


  }
  }
/*     {
      //get macro name

      size_t pos = value.find_first_of (':');
      if (pos == string::npos)
          pos = value.size();

      macroname = value.substr (0, pos);

      auto f = pool.macros.find (macroname);
      if (f != pool.macros.end())
         {
           CMacro *m = pool.macros[macroname]->create_self (value);

           if (m)
            { macros.insert (std::make_pair (macroname, m));
           //macros.insert (std::make_pair (macroname, pool.macros[macroname]->create_self (value)));

           //  cout << "macroname : " << macroname << " value: " << value << endl;
            }
         }
     }
*/
///
  rnd_length = 8;
  precision = 3;
  len_min = 0;
  len_max = 0;

  rnd_generator = new std::mt19937 (rnd_dev());
  vartype = get_value_nature (val);

  if (val.find ("@datetime") != string::npos && vartype != VT_SEQ)
     {
      vartype = VT_DATETIME;

      size_t pos = val.find (":");
      if (pos == string::npos)
         {
          //NOT VALUE
          return;
         }

       value = val.substr (pos + 1);

//      cout << "value: " << value << endl;
      }



  if (val.find ("@file_source") != string::npos)
     {
      vartype = VT_SEQ;

      //cout << "!!!! " <<  val << endl;

      size_t pos = val.find (":");
      if (pos == string::npos)
         {
          //NOT VALUE
          return;
         }

      string path = val.substr (pos + 1);
      string t1 = string_file_load (path);
      if (! t1.empty())
         {
          string t2 = string_replace_all (t1, "\n", "|");
          t2 = string_replace_all (t2, "\r\n", "|"); //for windows
          t2.pop_back(); //remove last |
          value = t2;

         }
      }


  if (vartype != VT_SEQ && val.find ("@int_random") != string::npos)
     {
      vector <string> vt = split_string_to_vector (value, ":");
      if (vt.size() == 2)
         {
          rnd_length = atoi (vt[1].c_str());
          value = "INTRNDMZ";
         }

       if (vt.size() == 3)
            {
             len_min = atoi (vt[1].c_str());
             len_max = atoi (vt[2].c_str());
             value = "INTRNDMZ";
            }

     }


  if (vartype != VT_SEQ && val.find ("@str_random") != string::npos)
     {
      vector <string> vt = split_string_to_vector (value, ":");
      if (vt.size() == 2)
         {
          rnd_length = atoi (vt[1].c_str());
          value = "STRRNDMZ";
         }

      if (vt.size() == 3)
         {
           len_min = atoi (vt[1].c_str());
           len_max = atoi (vt[2].c_str());
            value = "STRRNDMZ";
          }

     }


  if (vartype != VT_SEQ && val.find ("@str_path") != string::npos)
     {
      vector <string> vt = split_string_to_vector (value, ":");
      if (vt.size() == 4)
         {
          rnd_path_min = atoi (vt[1].c_str());
          rnd_path_max = atoi (vt[2].c_str());
          rnd_path_deep = atoi (vt[3].c_str());

          if (rnd_path_deep == 0)
             rnd_path_deep = 1;

          vartype = VT_SINGLE;
          value = "STRRNDPATH";
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


string CVar::gen_rnd_path (size_t min, size_t max, size_t deep)
{
  string result;

  size_t deep_max = get_rnd (1, deep);

  for (size_t d = 0; d < deep_max; d++)
      {
       result += "/";
       int len = get_rnd (min, max);
       result += gen_string (len);

      }

  return result;
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



string CVar::gen_random_ip()
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


string CVar::gen_number (size_t len)
{
  std::uniform_int_distribution<> distrib (0, 9);

  ostringstream st;

  for (size_t i = 0; i < len; i++)
      {
       st << distrib (*rnd_generator);
      }

  return st.str();
}


string CVar::gen_number (size_t min, size_t max)
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


string CVar::gen_string (size_t len)
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


string CVar::gen_string (size_t min, size_t max)
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



string CVar::get_datetime (const string &format)
{
  auto t = std::time (nullptr);
  auto tm = *std::localtime(&t);

  std::ostringstream oss;
  oss << std::put_time (&tm, format.c_str());

  auto result = oss.str();
  return result;
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
  else
  if (vartype == VT_DATETIME)
     result = get_datetime (v[0]);


   //pre process macros

   if (vartype == VT_SEQ)
     {
      if (result[0] == '@')
       { cout << "macro!"  << result << endl;
        macroname = get_macro_name (result);
      }

     }

   /*
   if (vartype == VT_SEQ)
     {
       if (result.find ("@datetime") != string::npos)
          {
           size_t pos = result.find (":");
          if (pos == string::npos)
            {
            //NOT VALUE
             return string();
            }

           result = result.substr (pos + 1);
           return get_datetime (result);
          }


      if (result.find ("@str_random") != string::npos)
         {
          // cout << "result: " << result << endl;

           vector <string> vt = split_string_to_vector (result, ":");

           if (vt.size() == 2)
            {
             rnd_length = atoi (vt[1].c_str());
             result = "STRRNDMZ";
            }

          if (vt.size() == 3)
            {
             len_min = atoi (vt[1].c_str());
             len_max = atoi (vt[2].c_str());
             result = "STRRNDMZ";
            }


         }

       if (result.find ("@int_random") != string::npos)
          {
           vector <string> vt = split_string_to_vector (result, ":");
           if (vt.size() == 2)
              {
               rnd_length = atoi (vt[1].c_str());
               result = "INTRNDMZ";
             }

          if (vt.size() == 3)
            {
             len_min = atoi (vt[1].c_str());
             len_max = atoi (vt[2].c_str());
             result = "INTRNDMZ";
            }


           }
      }
*/
   //handle macros


  if (result == "STRRNDPATH")
    {
  //   cout << "result == STRRNDPATH" << endl;
     return gen_rnd_path (rnd_path_min, rnd_path_max, rnd_path_deep);
    }



  if (result[0] == '@')
  {
     macroname = get_macro_name (result);

     if (! macroname.empty())
  {
   auto f = pool.macros.find (macroname);
   if (f != pool.macros.end())
      {
       //cout << "f->second->len_min:" << f->second->len_min  << endl;
       f->second->parse (result);
//      result = f->first;

     }


  }
  }

  if (! macroname.empty())
  {
   auto f = pool.macros.find (macroname);
   if (f != pool.macros.end())
      {
       //cout << "f->second->len_min:" << f->second->len_min  << endl;
       result = f->second->process();
//      result = f->first;

     }
  }

  return result;
}
