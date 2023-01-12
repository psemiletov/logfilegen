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


#include <sys/time.h>

#include "tpl.h"
#include "utl.h"


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



int CVar::get_rnd (int ta, int tb)
{
   std::uniform_int_distribution <> distrib (ta, tb);
   return distrib (*rnd_generator);
}


string CVar::gen_msecs()
{
  //cout << "fa:" << fa << " fb:" << fb << endl;
  std::uniform_real_distribution <> distrib (fa, fb);

  //cout << "---------" << endl;
  //cout << distrib (*rnd_generator) << endl;
  //cout << "---------" << endl;


  std::string s;
  std::stringstream sstream;
  sstream.setf(std::ios::fixed);
  sstream.precision (precision);
  sstream << distrib (*rnd_generator);

  return sstream.str();
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
  precision = 3;

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

  //bool float_range = false;
/*
  if (val.find ("$msec_random") != string::npos)
     {
      cout << "$msec_random" << endl;
      float_range = true;
      vector <string> vt = split_string_to_vector (value, ":");
      if (vt.size() == 2)
         value = vt[1];

      vartype = VT_FLOATRANGE;

      cout << "value: " << value << endl;

     }
*/
  if (vartype == VT_SINGLE)
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

          //get precision
          precision = v[0].size() - pos - 1;

         }
       else
           {
            a = atoi (v[0].c_str());
            b = atoi (v[1].c_str());
           }
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
  else
  if (vartype == VT_FLOATRANGE)
     result = gen_msecs();


   //handle macros

//   cout << "result: " << result << endl;

  if (result == "USER_WORD")
      return gen_word (rnd_length);

  if (result == "USER_NUMBER")
      return gen_number (rnd_length);

  if (result == "INTRNDMZ")
      return gen_number (rnd_length);

  if (result == "STRRNDMZ")
      return gen_word (rnd_length);

//  if (result == "MSECRNDMZ")
  //    return gen_msecs();


 // if (k.find ("$msec_random") != string::npos)
   //  return gen_msecs();


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


      vars.insert (std::make_pair ("$msec_random", new CVar ("$msec_random", "0.1..60.0")));
      vars.insert (std::make_pair ("$request_time", new CVar ("$request_time", "0.1..60.0")));
      vars.insert (std::make_pair ("$connection_time", new CVar ("$connection_time", "0.1..60.0")));

  //    vars.insert (std::make_pair ("$msec", new CVar ("$msec", "1.0..60.0")));




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


//change to     for (auto it = mymap.rbegin(); it != mymap.rend(); it++)

   map <string, CVar*>::reverse_iterator it;
   for (it = vars.rbegin(); it != vars.rend(); it++)
      {
       string variable = it->first;
       string replacement = it->second->get_val();

//       cout << "variable: " << variable << " ^^^^ replacement: " << replacement << endl;
//#pragma omp critical
       str_replace (logstring, variable, replacement);
      }


  return logstring;
}


/*
struct less_than_key
{
    inline bool operator() (const CVar& v1, const CVar& v2)
    {
        return (v1.key < v2.key);
    }
};
*/


struct less_than_key
{
    inline bool operator() (CVar *v1, const CVar *v2)
    {
        return (v1->k < v2->k);
    }
};


CTpl2::CTpl2 (const string &fname, const string &amode)
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




      vars.push_back (new CVar ("$logstring", logstrings["nginx"]));

      vars.push_back (new CVar ("$remote_addr", "IP_RANDOM"));
      vars.push_back (new CVar ("$remote_user", "USER_WORD|USER_NUMBER"));
      vars.push_back (new CVar ("$time_local", "%d/%b/%Y:%H:%M:%S %z"));
//      vars.insert (std::make_pair ("$time_iso8601", new CVar ("%Y-%m-%dT%H:%M:%SZ"))); //don't redefine

      vars.push_back (new CVar ("$request", "GET|POST|PUT|PATCH|DELETE"));
      vars.push_back (new CVar ("$uri", "/|/favico.ico|/doc"));
      vars.push_back (new CVar ("$document_uri", "/|/favico.ico|/doc"));
      vars.push_back (new CVar ("$protocol", "HTTP/1.1"));
      vars.push_back (new CVar ("$status", "200|400"));
      vars.push_back (new CVar ("$body_bytes_sent", "1..9999"));
      vars.push_back (new CVar ("$http_referer", "-"));
      vars.push_back (new CVar ("$http_user_agent", "Mozilla|Chrome|Vivaldi|Opera"));

      //SORT HERE

   //     std::sort (vars.begin(), vars.end(), less_than_key());

    //   std::sort(vars.begin(), vars.end(),greater<CVar>());
      //LOOP


      /*

        std::vector<Type> v = ....;
std::string myString = ....;
auto it = find_if(v.begin(), v.end(), [&myString](const Type& obj) {return obj.getName() == myString;})

if (it != v.end())
{
  // found element. it is an iterator to the first matching element.
  // if you really need the index, you can also get it:
  auto index = std::distance(v.begin(), it);
}

       */
/*
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

*/
//   sort (vars.begin(), vars.end(), greater<string>());

}
}


string CTpl2::prepare_log_string()
{
/*
  string logstring = vars["$logstring"]->get_val();
  if (logstring.empty())
    {
      cout << "$logstring mandatory variable is not defined!" << endl;
      return "";
    }
*/

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
/*
 *
 *
   for (auto itr = vars.end(); itr != vars.begin(); itr--)
      {
       string variable = itr->first;
       string replacement = itr->second->get_val();

//       cout << "variable: " << variable << " ^^^^ replacement: " << replacement << endl;
//#pragma omp critical
       str_replace (logstring, variable, replacement);
      }

*/

//for (auto it = begin (vector); it != end (vector); ++it) {
  //  it->doSomething ();
//}


//change to     for (auto it = mymap.rbegin(); it != mymap.rend(); it++)
/*
   map <string, CVar*>::reverse_iterator it;
   for (it = vars.rbegin(); it != vars.rend(); it++)
      {
       string variable = it->first;
       string replacement = it->second->get_val();

//       cout << "variable: " << variable << " ^^^^ replacement: " << replacement << endl;
//#pragma omp critical
       str_replace (logstring, variable, replacement);
      }
*/

  return string(); //logstring;
}



CTpl2::~CTpl2()
{
  delete pf;

  for (auto itr = vars.begin(); itr != vars.end(); ++itr)
      {
       delete (*itr);
      //  cout << itr->first
        //     << '\t' << itr->second << '\n';
      }
}
