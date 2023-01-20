#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <chrono>


#include <sys/time.h>

#include "tpl.h"
#include "utl.h"


//full nginx vars see at
//http://nginx.org/en/docs/varindex.html


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
}


int CVar::get_rnd (int ta, int tb)
{
   std::uniform_int_distribution <> distrib (ta, tb);
   return distrib (*rnd_generator);
}


string CVar::gen_rnd_path (size_t min, size_t max, size_t deep)
{
  string result;

//  result += "/";

  size_t deep_max = get_rnd (1, deep);

  for (size_t d = 0; d < deep_max; d++)
      {
       result += "/";
       int len = get_rnd (min, max);
       result += gen_word (len);

      }

 // cout << "RES " << result << endl;

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


   //handle macros


//  if (result == "USER_WORD")
  //    return gen_word (rnd_length);

//  if (result == "USER_NUMBER")
  //    return gen_number (rnd_length);

  if (result == "INTRNDMZ" && len_max == 0)
      return gen_number (rnd_length);

  if (result == "INTRNDMZ" && len_max != 0)
      return gen_number (len_min, len_max);

  if (result == "STRRNDMZ" && len_max == 0)
      return gen_word (rnd_length);

  if (result == "STRRNDMZ" && len_max != 0)
      return gen_string (len_min, len_max);


  if (result == "STRRNDPATH")
    {
  //   cout << "result == STRRNDPATH" << endl;
     return gen_rnd_path (rnd_path_min, rnd_path_max, rnd_path_deep);
    }



  if (result == "@ip_random")
     result = gen_random_ip();

  return result;
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


string CVar::gen_word (size_t len)
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


void CTpl::replace_value_by_key (const string &key, const string &value)
{
  auto f = vars.find (key);
  if (f != vars.end()) //delete if variable pre-defined
     {
      delete f->second;
      vars.erase (f);
      vars.insert (std::make_pair (key, new CVar (key, value)));
     }
}



CTpl::~CTpl()
{
  delete pf;

  for (auto itr = vars.begin(); itr != vars.end(); ++itr)
      {
       delete (itr->second);
      }
}


CTpl::CTpl (const string &fname, const string &amode)
{

  pf = new CPairFile (fname, false);

  mode = amode;

  logstrings["nginx"] = "$remote_addr - $remote_user [$time_local] \"$request $uri $protocol\" $status $body_bytes_sent \"$http_referer\" \"$http_user_agent\"";

  //see https://httpd.apache.org/docs/2.4/mod/mod_log_config.html
  logstrings["apache"] = "\"%h %l %u %t \"%r\" %>s %b\"";


  //DEFAULTS

  if (mode == "nginx")
     {
      vars.insert (std::make_pair ("$logstring", new CVar ("$logstring", logstrings["nginx"])));

      vars.insert (std::make_pair ("$body_bytes_sent", new CVar ("$body_bytes_sent", "1..9999")));
      vars.insert (std::make_pair ("$connection_time", new CVar ("$connection_time", "0.001..60.000")));
      vars.insert (std::make_pair ("$document_uri", new CVar ("$document_uri", "/|/favico.ico|/doc")));
      vars.insert (std::make_pair ("$http_referer", new CVar ("$http_referer", "-")));
      vars.insert (std::make_pair ("$http_user_agent", new CVar ("$http_user_agent", "Mozilla|Chrome|Vivaldi|Opera")));
      vars.insert (std::make_pair ("$seconds_random", new CVar ("$seconds_random", "0.001..60.000")));
      vars.insert (std::make_pair ("$protocol", new CVar ("$protocol", "HTTP/1.1")));
      vars.insert (std::make_pair ("$remote_addr", new CVar ("$remote_addr", "IP_RANDOM")));
      vars.insert (std::make_pair ("$remote_user", new CVar ("$remote_user", "USER_WORD|USER_NUMBER")));
      vars.insert (std::make_pair ("$request", new CVar ("$request", "GET|POST|PUT|PATCH|DELETE")));
      vars.insert (std::make_pair ("$request_time", new CVar ("$request_time", "0.001..60.000")));
      vars.insert (std::make_pair ("$status", new CVar ("$status", "200|400")));
      vars.insert (std::make_pair ("$time_iso8601", new CVar ("$time_iso8601", "%Y-%m-%dT%H:%M:%SZ"))); //don't redefine
      vars.insert (std::make_pair ("$time_local", new CVar ("$time_local", "%d/%b/%Y:%H:%M:%S %z")));
      vars.insert (std::make_pair ("$uri", new CVar ("$uri", "/|/favico.ico|/doc")));
     }


  for (map <string, string>::const_iterator it = pf->values.begin(); it != pf->values.end(); it++)
      {
       auto f = vars.find (it->first);
       if (f != vars.end()) //delete if variable pre-defined
          {
           delete f->second;
           vars.erase (f);
          }

      vars.insert (std::make_pair (it->first, new CVar (it->first, it->second)));
     }

}

/*
string CTpl::prepare_log_string()
{

  string logstring = vars["$logstring"]->get_val();

  if (logstring.empty())
     {
      cout << "$logstring mandatory variable is not defined!" << endl;
      return "";
     }

//change to     for (auto it = mymap.rbegin(); it != mymap.rend(); it++)
  map <string, CVar*>::reverse_iterator it;
  for (it = vars.rbegin(); it != vars.rend(); it++)
      {
       string variable = it->first;
       string replacement = it->second->get_val();
       str_replace (logstring, variable, replacement);
      }

  return logstring;
}
*/



string CTpl::prepare_log_string()
{

  string logstring = vars["$logstring"]->get_val();

  if (logstring.empty())
     {
      cout << "$logstring mandatory variable is not defined!" << endl;
      return "";
     }

//change to     for (auto it = mymap.rbegin(); it != mymap.rend(); it++)
  map <string, CVar*>::reverse_iterator it;
  for (it = vars.rbegin(); it != vars.rend(); it++)
      {
       string variable = it->first;
       //string replacement = it->second->get_val();

       //str_replace (logstring, variable, replacement);


       //string result = s;
       size_t i = 0;
       do
         {
          i = logstring.find (variable);
          if (i != string::npos)
             logstring = logstring.replace (i, variable.length(), it->second->get_val());
         }
       while (i != string::npos);


      }

  return logstring;
}
