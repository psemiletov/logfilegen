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


//full nginx vars see at
//http://nginx.org/en/docs/varindex.html

/*

 209.124.125.232 - abcd1234 [22/Dec/2022:18:56:54 +0000] "GET /api HTTP/1.1" 403 1234 "static text"
209.124.125.132 - 5678efgi [22/Dec/2022:18:56:55 +0000] "GET /docs HTTP/2.0" 200 5678 "static text"

 */

#define VN_SINGLE 1
#define VN_SEQ 2
#define VN_RANGE 3


int get_value_nature (const string &s)
{
  if (s.find ("|") != string::npos)
     return VN_SEQ;

  if (s.find ("-") != string::npos)
     return VN_RANGE;

  return VN_SINGLE;
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


vector <string> split_string_to_vector (const string &s, char delimeter)
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

 logstrings["nginx"] = "$remote_addr - $remote_user [$time_local] \"$request$uri$protocol\" $status $body_bytes_sent \"$http_referer\" \"$http_user_agent\"";



  tlogstring  = get_string ("$logstring", logstrings[mode]);

  ip = get_string ("$remote_addr", "IP_RANDOM");

  user = get_string ("$remote_user", "WORD|NUMBER"); //WORD|NUMBER or -

   //ADD TIMESTAMP macro

  datetime = get_string ("$time_local", "%x:%X");

  request = get_string ("$request", "GET|POST|PUT|PATCH|DELETE");

  v_request = split_string_to_vector (request, '|');

  //add more options
  uri = get_string ("$uri", " /");

  int nv = get_value_nature (uri);



  nv = get_value_nature (status);

  if (nv == VN_SEQ)
      v_status = split_string_to_vector (status, '|');

  if (nv == VN_RANGE)
      v_status = split_string_to_vector (status, '-');





  body_bytes_sent = get_string ("$body_bytes_sent", "100-10000");

  nv = get_value_nature (body_bytes_sent);

  if (nv == VN_RANGE)
      v_body_bytes_sent = split_string_to_vector (body_bytes_sent, '-');



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

  if (nv == VN_SEQ)
      v_status = split_string_to_vector (status, '|');

  if (nv == VN_RANGE)
      v_status = split_string_to_vector (status, '-');



  //Must be more complex, i.e
  //"GET / HTTP/1.1"
  ///favicon.ico HTTP/1.1"


  if (v_request.size() == 1)
     str_replace (logstring, "$request", v_request[0]);
  else
      str_replace (logstring, "$request", v_request[get_rnd (0, v_request.size()-1)]);

  //////////////


  str_replace (logstring, "$uri", uri);

  protocol = get_string ("$protocol", " HTTP/1.1");
  str_replace (logstring, "$protocol", protocol);


//////////////


  nv = get_value_nature (status);

  if (nv == VN_SINGLE)
    str_replace (logstring, "$status", status);


  if (nv == VN_SEQ)
     {
      str_replace (logstring, "$status", v_status[get_rnd (0, v_status.size()-1)]);
     }

  if (nv == VN_RANGE)
     {
      int a = atoi (v_status[0].c_str());
      int b = atoi (v_status[1].c_str()) + 1;

      str_replace (logstring, "$status", std::to_string (get_rnd (a, b)));
     }



  body_bytes_sent = get_string ("$body_bytes_sent", "100-10000");

  nv = get_value_nature (body_bytes_sent);

  if (nv == VN_SINGLE)
      str_replace (logstring, "$body_bytes_sent", body_bytes_sent);

  if (nv == VN_RANGE)
      {
      int a = atoi (v_body_bytes_sent[0].c_str());
      int b = atoi (v_body_bytes_sent[1].c_str()) + 1;

      str_replace (logstring, "$body_bytes_sent", std::to_string (get_rnd (a, b)));
     }


  return logstring;
};

/*
string CTpl::prepare_log_string()
{


  string logstring = tlogstring;

//  ip = get_string ("IP", "1111.1111.1111.1111");
//  cout << "ip = get_string IP: " << ip << endl;


  //logstring.replace (logstring.find("IP"), string("IP").size(), ip);

  //logstring.replace (logstring.find("COOL"), string("COOL").size(), "^^^^^^^^^^^^^^^^^^^^^^^^^^^");


  logstring.replace (logstring.find("RND_IP"), string("RND_IP").size(), gen_random_ip());

  user = get_string ("USER", "WORD|NUMBER");

  if (user == "NUMBER")
    //logstring.replace(logstring.find("USER"), string("USER").size(), gen_user_number(8));
    str_replace (logstring, "USER", gen_user_number(8));
  else
  if (user == "WORD")
     //logstring.replace(logstring.find("USER"), string("USER").size(), gen_user_word(8));
      str_replace (logstring, "USER", gen_user_word(8));
  else
      {
       //get random
       if (get_rnd (0, 1) == 0)
          //logstring.replace(logstring.find("USER"), string("USER").size(), gen_user_number(8));

          str_replace (logstring, "USER", gen_user_number(8));
       else
          str_replace (logstring, "USER", gen_user_word(8));
      //    logstring.replace(logstring.find("USER"), string("USER").size(), gen_user_word(8));

      }


   //cout << "v[0]:" << v[0] << endl;

  //ADD TIMESTAMP macro

  datetime = get_string ("DATETIME", "%x:%X");
  logstring.replace(logstring.find("DATETIME"), string("DATETIME").size(), get_datetime (datetime));
  //add msecs support


    //vector <string> v = split_string_to_vector (user, '|');
  //if (v.size() == 1)


//  st << distrib (*rnd_generator);

  string request = get_string ("REQUEST", "GET|POST|PUT|PATCH|DELETE");
  vector <string> vreq = split_string_to_vector (request, '|');
  if (vreq.size() == 1)
     logstring.replace(logstring.find("REQUEST"), string("REQUEST").size(), vreq[0]);
  else
      logstring.replace(logstring.find("REQUEST"), string("REQUEST").size(), vreq[get_rnd (0, vreq.size()-1)]);


  return logstring;
};
*/
