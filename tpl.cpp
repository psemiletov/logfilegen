#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <chrono>

//#include <omp.h>

#include <sys/time.h>

#include "tpl.h"


//full nginx vars see at
//http://nginx.org/en/docs/varindex.html


int get_value_nature (const string &s)
{
  if (s.find ("|") != string::npos)
     return VT_SEQ;

  if (s.find ("..") != string::npos)
     return VT_RANGE;

  return VT_SINGLE;
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


vector <string> split_string_to_vector (const string& s, const string& delimeter, const bool keep_empty = true)
{
  vector <string> result;

  if (delimeter.empty())
     {
      result.push_back(s);
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

/*
vector <string> split_string_to_vector2 (const string &s, char delimeter)
{
  string ts;
  stringstream ss (s);
  vector <string> tokens;

  while (getline (ss, ts, delimeter))
        {
         tokens.push_back (ts);
        }

  return tokens;
}
*/


int CVar::get_rnd (int ta, int tb)
{
   std::uniform_int_distribution <> distrib (ta, tb);
   return distrib (*rnd_generator);
}


CVar::~CVar()
{
  delete rnd_generator;
}


CVar::CVar (const string &key, const string &val)
{
  k = key;
  string value = val;
  rnd_length = 8;

//  cout << "CVar::CVar === key: " << key << " value:" << val << endl;

  rnd_generator = new std::mt19937 (rnd_dev());

  vartype = get_value_nature (val);

  if (val.find ("$int_random") != string::npos)
     {
      vector <string> vt = split_string_to_vector (value, ":");
      if (vt.size() == 2)
        {
          rnd_length = atoi (vt[1].c_str());
          value = "INTRNDMZ";
        }

     }

  if (val.find ("$str_random") != string::npos)
     {
      vector <string> vt = split_string_to_vector (value, ":");
      if (vt.size() == 2)
        {
          rnd_length = atoi (vt[1].c_str());
          value = "STRRNDMZ";
        }

     }


  if (vartype == VT_SINGLE)
     v.push_back (value);
  else
  if (vartype == VT_SEQ)
      v = split_string_to_vector (value, "|");
  else
  if (vartype == VT_RANGE)
     {
       v = split_string_to_vector (value, "..");
       a = atoi (v[0].c_str());
       b = atoi (v[1].c_str()) + 1;
     }




}

/*
std::string currentISO8601TimeUTC() {
  auto now = std::chrono::system_clock::now();
  auto itt = std::chrono::system_clock::to_time_t(now);
  std::ostringstream ss;
  ss << std::put_time(gmtime(&itt), "%FT%TZ");
  return ss.str();
}
*/


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

   //handle macros

  if (result == "USER_WORD")
      return gen_word (rnd_length);

  if (result == "USER_NUMBER")
      return gen_number (rnd_length);

  if (result == "INTRNDMZ")
      return gen_number (rnd_length);

  if (result == "STRRNDMZ")
      return gen_word (rnd_length);




  if (k.find ("$int_random") != string::npos)
    {
     int i = atoi (v[0].c_str());
     if (i > 0)
         result = gen_number (i);

     return result;
    }

  if (k.find ("$str_random") != string::npos)
    {
     int i = atoi (v[0].c_str());
     if (i > 0)
         result = gen_word (i);

     return result;
    }


  //assuming date time

  if (k.find ("$time_") != string::npos)
  //if (result.find ("%") != string::npos)
     result = get_datetime (result);

  if (result == "IP_RANDOM")
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
  std::uniform_int_distribution<> distrib(0, 9);

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

  std::uniform_int_distribution<> distrib(0, 25);

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


CTpl::~CTpl()
{
  delete pf;

  for (auto itr = vars.begin(); itr != vars.end(); ++itr)
      {
       delete (itr->second);
      //  cout << itr->first
        //     << '\t' << itr->second << '\n';
      }
}


CTpl::CTpl (const string &fname, const string &amode)
{

  pf = new CPairFile (fname, false);

  mode = amode;


  logstrings["nginx"] = "$remote_addr - $remote_user [$time_local] \"$request $uri $protocol\" $status $body_bytes_sent \"$http_referer\" \"$http_user_agent\"";

  //see https://httpd.apache.org/docs/2.4/mod/mod_log_config.html
  logstrings["apache"] = "\"%h %l %u %t \"%r\" %>s %b\"";

  /*

   Common Log Format (CLF)
"%h %l %u %t \"%r\" %>s %b"
Common Log Format with Virtual Host
"%v %h %l %u %t \"%r\" %>s %b"
NCSA extended/combined log format
"%h %l %u %t \"%r\" %>s %b \"%{Referer}i\" \"%{User-agent}i\""
Referer log format
"%{Referer}i -> %U"
Agent (Browser) log format
"%{User-agent}i"
You can use the %{format}t directive multiple times to build up a time format using the extended format tokens like msec_frac:

Timestamp including milliseconds
"%{%d/%b/%Y %T}t.%{msec_frac}t %{%z}t"

   */

  //string ls = pf->get_string("$logstring", logstrings[mode]);

  //DEFAULTS

  if (mode == "nginx")
     {

      vars.insert (std::make_pair ("$logstring", new CVar ("$logstring", logstrings["nginx"])));

      vars.insert (std::make_pair ("$remote_addr", new CVar ("$remote_addr", "IP_RANDOM")));
      vars.insert (std::make_pair ("$remote_user", new CVar ("$remote_user", "USER_WORD|USER_NUMBER")));
      vars.insert (std::make_pair ("$time_local", new CVar ("$time_local", "%d/%b/%Y:%H:%M:%S %z")));
//      vars.insert (std::make_pair ("$time_iso8601", new CVar ("%Y-%m-%dT%H:%M:%SZ"))); //don't redefine

      vars.insert (std::make_pair ("$request", new CVar ("$request", "GET|POST|PUT|PATCH|DELETE")));
      vars.insert (std::make_pair ("$uri", new CVar ("$uri", "/|/favico.ico|/doc")));
      vars.insert (std::make_pair ("$document_uri", new CVar ("$document_uri", "/|/favico.ico|/doc")));
      vars.insert (std::make_pair ("$protocol", new CVar ("$protocol", "HTTP/1.1")));
      vars.insert (std::make_pair ("$status", new CVar ("$status", "200|400")));
      vars.insert (std::make_pair ("$body_bytes_sent", new CVar ("$body_bytes_sent", "1..9999")));
      vars.insert (std::make_pair ("$http_referer", new CVar ("$http_referer", "-")));
      vars.insert (std::make_pair ("$http_user_agent", new CVar ("$http_user_agent", "Mozilla|Chrome|Vivaldi|Opera")));
     }


  for (map <string, string>::const_iterator it = pf->values.begin(); it != pf->values.end(); it++)
     {
      auto f = vars.find (it->first);
      if (f != vars.end())
        {
         delete f->second;
         vars.erase (f);
        }

      vars.insert (std::make_pair (it->first, new CVar (it->first, it->second)));
    }


}



string CTpl::prepare_log_string()
{

  string logstring = vars["$logstring"]->get_val();
  if (logstring.empty())
    {
      cout << "$logstring mandatory variable is not defined!" << endl;
      return "";
    }


//#pragma omp parallel for
/*
for (auto itr = vars.begin(); itr != vars.end(); ++itr)
      {
       string variable = itr->first;
       string replacement = itr->second->get_val();

//       cout << "variable: " << variable << " ^^^^ replacement: " << replacement << endl;
//#pragma omp critical
       str_replace (logstring, variable, replacement);
      }
*/

   for (auto itr = vars.end(); itr != vars.begin(); --itr)
      {
       string variable = itr->first;
       string replacement = itr->second->get_val();

//       cout << "variable: " << variable << " ^^^^ replacement: " << replacement << endl;
//#pragma omp critical
       str_replace (logstring, variable, replacement);
      }




  return logstring;
}
