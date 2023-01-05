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


#include <memory>


#include <sys/time.h>

#include "tpl.h"


//full nginx vars see at
//http://nginx.org/en/docs/varindex.html

/*

 209.124.125.232 - abcd1234 [22/Dec/2022:18:56:54 +0000] "GET /api HTTP/1.1" 403 1234 "static text"
209.124.125.132 - 5678efgi [22/Dec/2022:18:56:55 +0000] "GET /docs HTTP/2.0" 200 5678 "static text"

 */

/*
#define VT_SINGLE 1
#define VT_SEQ 2
#define VT_RANGE 3
*/

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


int CTpl::get_rnd (int a, int b)
{
   std::uniform_int_distribution <> distrib (a, b);
   return distrib (*rnd_generator);
}



string CTpl::gen_random_ip()
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



string CTpl::gen_user_number (size_t len)
{
  std::uniform_int_distribution<> distrib(0, 9);

  ostringstream st;

  for (size_t i = 0; i < len; i++)
      {
       st << distrib (*rnd_generator);
      }

  return st.str();
}


string CTpl::gen_user_word (size_t len)
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

/*

%a	Abbreviated weekday name *	Thu
%A	Full weekday name *	Thursday
%b	Abbreviated month name *	Aug
%B	Full month name *	August
%c	Date and time representation *	Thu Aug 23 14:55:02 2001
%C	Year divided by 100 and truncated to integer (00-99)	20
%d	Day of the month, zero-padded (01-31)	23
%D	Short MM/DD/YY date, equivalent to %m/%d/%y	08/23/01
%e	Day of the month, space-padded ( 1-31)	23
%F	Short YYYY-MM-DD date, equivalent to %Y-%m-%d	2001-08-23
%g	Week-based year, last two digits (00-99)	01
%G	Week-based year	2001
%h	Abbreviated month name * (same as %b)	Aug
%H	Hour in 24h format (00-23)	14
%I	Hour in 12h format (01-12)	02
%j	Day of the year (001-366)	235
%m	Month as a decimal number (01-12)	08
%M	Minute (00-59)	55
%n	New-line character ('\n')
%p	AM or PM designation	PM
%r	12-hour clock time *	02:55:02 pm
%R	24-hour HH:MM time, equivalent to %H:%M	14:55
%S	Second (00-61)	02
%t	Horizontal-tab character ('\t')
%T	ISO 8601 time format (HH:MM:SS), equivalent to %H:%M:%S	14:55:02
%u	ISO 8601 weekday as number with Monday as 1 (1-7)	4
%U	Week number with the first Sunday as the first day of week one (00-53)	33
%V	ISO 8601 week number (00-53)	34
%w	Weekday as a decimal number with Sunday as 0 (0-6)	4
%W	Week number with the first Monday as the first day of week one (00-53)	34
%x	Date representation *	08/23/01
%X	Time representation *	14:55:02
%y	Year, last two digits (00-99)	01
%Y	Year	2001
%z	ISO 8601 offset from UTC in timezone (1 minute=1, 1 hour=100)
If timezone cannot be termined, no characters	+100
%Z	Timezone name or abbreviation *
If timezone cannot be termined, no characters	CDT
%%	A % sign	%

*/

string CTpl::get_datetime (const string &format)
{
  auto t = std::time (nullptr);
  auto tm = *std::localtime(&t);

  std::ostringstream oss;
  oss << std::put_time (&tm, format.c_str());

  auto result = oss.str();
  return result;
}


string get_datetime_with_msecs (const string &format)
{
    using namespace std::chrono;

    auto now = system_clock::now();

    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    auto timer = system_clock::to_time_t(now);

    std::tm bt = *std::localtime(&timer);

    std::ostringstream oss;

    oss << std::put_time (&bt, format.c_str());
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}


CTpl::CTpl (const string &fname, const string &amode): CPairFile (fname, false)
{
  mode = amode;

  rnd_generator = new std::mt19937 (rnd_dev());

  logstrings["nginx"] = "$remote_addr - $remote_user [$time_local] \"$request $uri $protocol\" $status $body_bytes_sent \"$http_referer\" \"$http_user_agent\"";



  tlogstring  = get_string ("$logstring", logstrings[mode]);

  ip = get_string ("$remote_addr", "IP_RANDOM");

  user = get_string ("$remote_user", "WORD|NUMBER"); //WORD|NUMBER or -

   //ADD TIMESTAMP macro

  //add $time_iso8601
  //add $time_custom with user-defined format instead of below

  //%d/%b/%Y:%H:%M:%S %z

  //datetime = get_string ("$time_local", "%x:%X");
  datetime = get_string ("$time_local", "%d/%b/%Y:%H:%M:%S %z");



  request = get_string ("$request", "GET|POST|PUT|PATCH|DELETE");

  v_request = split_string_to_vector (request, "|");

  //add more options
  uri = get_string ("$uri", "/|/favico.ico|/doc");

  int nv = get_value_nature (uri);

  if (nv == VT_SEQ)
      v_uri = split_string_to_vector (uri, "|");


  protocol = get_string ("$protocol", "HTTP/1.1");

  nv = get_value_nature (protocol);
  if (nv == VT_SEQ)
      v_protocol = split_string_to_vector (protocol, "|");


  status = get_string ("$status", "200|404");

  nv = get_value_nature (status);

  if (nv == VT_SEQ)
      v_status = split_string_to_vector (status, "|");

  if (nv == VT_RANGE)
      v_status = split_string_to_vector (status, "..");




  body_bytes_sent = get_string ("$body_bytes_sent", "100..10000");

  nv = get_value_nature (body_bytes_sent);

  if (nv == VT_RANGE)
      v_body_bytes_sent = split_string_to_vector (body_bytes_sent, "..");


  http_referer = get_string ("$http_referer", "-");


  http_user_agent = get_string ("$http_user_agent", "Mozilla|Chrome|Vivaldi|Opera");
  nv = get_value_nature (http_user_agent);

  if (nv == VT_SEQ)
      v_http_user_agent = split_string_to_vector (http_user_agent, "|");



}


CTpl::~CTpl()
{
  delete rnd_generator;
}






string CTpl::prepare_log_string()
{

  string logstring = tlogstring;


  if (ip == "IP_RANDOM")
     logstring.replace (logstring.find ("$remote_addr"), string ("$remote_addr").size(), gen_random_ip());
  else
      logstring.replace (logstring.find ("$remote_addr"), string ("$remote_addr").size(), ip);


  if (user == "NUMBER")
    str_replace (logstring, "$remote_user", gen_user_number (8));
  else
  if (user == "WORD")
      str_replace (logstring, "$remote_user", gen_user_word (8));
  else
      {
       if (get_rnd (0, 1) == 0)
          str_replace (logstring, "$remote_user", gen_user_number (8));
       else
          str_replace (logstring, "$remote_user", gen_user_word (8));
      }


  str_replace (logstring, "$time_local", get_datetime (datetime));



  int nv = get_value_nature (status);

  if (nv == VT_SEQ)
      v_status = split_string_to_vector (status, "|");

  if (nv == VT_RANGE)
      v_status = split_string_to_vector (status, "..");



  //Must be more complex, i.e
  //"GET / HTTP/1.1"
  ///favicon.ico HTTP/1.1"


  if (v_request.size() == 1)
     str_replace (logstring, "$request", v_request[0]);
  else
      str_replace (logstring, "$request", v_request[get_rnd (0, v_request.size()-1)]);

  //////////////

  nv = get_value_nature (uri);

  if (nv == VT_SEQ)
      str_replace (logstring, "$uri", v_uri[get_rnd (0, v_uri.size()-1)]);
   else
      str_replace (logstring, "$uri", uri);



  nv = get_value_nature (protocol);
  if (nv == VT_SEQ)
      str_replace (logstring, "$uri", v_protocol[get_rnd (0, v_protocol.size()-1)]);
   else
//  protocol = get_string ("$protocol", "HTTP/1.1");
       str_replace (logstring, "$protocol", protocol);


//////////////


  nv = get_value_nature (status);

  if (nv == VT_SINGLE)
    str_replace (logstring, "$status", status);

  if (nv == VT_SEQ)
      str_replace (logstring, "$status", v_status[get_rnd (0, v_status.size()-1)]);

  if (nv == VT_RANGE)
     {
      int a = atoi (v_status[0].c_str());
      int b = atoi (v_status[1].c_str()) + 1;

      str_replace (logstring, "$status", std::to_string (get_rnd (a, b)));
     }




  nv = get_value_nature (body_bytes_sent);

  if (nv == VT_SINGLE)
      str_replace (logstring, "$body_bytes_sent", body_bytes_sent);

  if (nv == VT_RANGE)
      {
      int a = atoi (v_body_bytes_sent[0].c_str());
      int b = atoi (v_body_bytes_sent[1].c_str()) + 1;

      str_replace (logstring, "$body_bytes_sent", std::to_string (get_rnd (a, b)));
     }


  str_replace (logstring, "$http_referer", http_referer);


   nv = get_value_nature (http_user_agent);

  if (nv == VT_SINGLE)
    str_replace (logstring, "$http_user_agent", status);


  if (nv == VT_SEQ)
      str_replace (logstring, "$http_user_agent", v_http_user_agent[get_rnd (0, v_http_user_agent.size()-1)]);


  return logstring;
};



int CVar::get_rnd (int ta, int tb)
{
   std::uniform_int_distribution <> distrib (ta, tb);
   return distrib (*rnd_generator);
}


//CVar::CVar (const string &val)

CVar::~CVar()
{
  delete rnd_generator;

}


CVar::CVar (const string &val)
{
  rnd_generator = new std::mt19937 (rnd_dev());

  vartype = get_value_nature (val);

  if (vartype == VT_SINGLE)
     v.push_back (val);
  else
  if (vartype == VT_SEQ)
      v = split_string_to_vector (val, "|");
  else
  if (vartype == VT_RANGE)
     {
       v = split_string_to_vector (val, "..");
       a = atoi (v[0].c_str());
       b = atoi (v[1].c_str()) + 1;
     }

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

   //handle macros

  //assuming date time
  if (result.find ("%") != string::npos)
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





CTpl2::~CTpl2()
{
  delete pf;
//  delete rnd_generator;
/*
  for( map <string, CVar*> itr = vars.begin(); itr != vars.end(); itr++)
    {
        delete (vars->second);
    }
*/
     //vars.clear();


     for (auto itr = vars.begin(); itr != vars.end(); ++itr) {
      //  cout << itr->first
        //     << '\t' << itr->second << '\n';
         delete (itr->second);

    }

}




CTpl2::CTpl2 (const string &fname, const string &amode)
{

cout <<   "CTpl2::CTpl2 (const string &fname, const string &amode) 11111111111111" << endl;


  pf = new CPairFile (fname, false);

  mode = amode;

  //rnd_generator = new std::mt19937 (rnd_dev());

  logstrings["nginx"] = "$remote_addr - $remote_user [$time_local] \"$request $uri $protocol\" $status $body_bytes_sent \"$http_referer\" \"$http_user_agent\"";



  //DEFAULTS

    vars.insert (std::make_pair ("$logstring", new CVar (logstrings[mode])));
    vars.insert (std::make_pair ("$remote_addr", new CVar ("IP_RANDOM")));
    vars.insert (std::make_pair ("$remote_user", new CVar ("WORD|NUMBER")));
    vars.insert (std::make_pair ("$time_local", new CVar ("%d/%b/%Y:%H:%M:%S %z")));
    vars.insert (std::make_pair ("$request", new CVar ("GET|POST|PUT|PATCH|DELETE")));
    vars.insert (std::make_pair ("$uri", new CVar ("/|/favico.ico|/doc")));
    vars.insert (std::make_pair ("$protocol", new CVar ("HTTP/1.1")));
    vars.insert (std::make_pair ("$status", new CVar ("1..9999")));
    vars.insert (std::make_pair ("$body_bytes_sent", new CVar ("1..9999")));
    vars.insert (std::make_pair ("$http_referer", new CVar ("-")));
    vars.insert (std::make_pair ("$remote_addr", new CVar ("IP_RANDOM")));
    vars.insert (std::make_pair ("$http_user_agent", new CVar ("Mozilla|Chrome|Vivaldi|Opera")));



// C++98
  // for (map <string, string>::const_iterator it = pf->values.begin(); it != pf->values.end(); it++)
      //std::cout << it->first << " = " << it->second << "; ";
    //   vars[it->first] = CVar (it->second, rnd_generator);

cout <<   "CTpl2::CTpl2 (const string &fname, const string &amode) 222222222222" << endl;


}



string CTpl2::prepare_log_string()
{

  string logstring = vars["$logstring"]->get_val();
  if (logstring.empty())
    {
      cout << "$logstring mandatory variable is not defined!" << endl;
      return "";
    }


   for (auto itr = vars.begin(); itr != vars.end(); ++itr)
      {
       string variable = itr->first;
       string replacement = itr->second->get_val();
      //  cout << itr->first
        //     << '\t' << itr->second << '\n';

    str_replace (logstring, variable, replacement);


    }



  return logstring;

}
