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

#include "tpl.h"


/*

IP={{ RANDOM IP }}
USER={{ WORD | NUMBER }}
DATETIME={{dd/mm/yyyy:hh:mm:ss z}}
REQUEST=["GET", "POST"]
URI=["api", "docs"]
PROTOCOL=["HTTP/1.1", "HTTP/2.0"]
STATUS=[200, 403]
BYTES=[0-5000]
STATIC_TEXT="static text"



 {{ IP }} - {{ USER }} [{{ DATETIME }}] "{{ REQUEST }} /{{ URI }} {{ PROTOCOL }}" {{ STATUS }} {{ BYTES }} "{{ STATIC_TEXT }}"


 209.124.125.232 - abcd1234 [22/Dec/2022:18:56:54 +0000] "GET /api HTTP/1.1" 403 1234 "static text"
209.124.125.132 - 5678efgi [22/Dec/2022:18:56:55 +0000] "GET /docs HTTP/2.0" 200 5678 "static text"

 */


string gen_random_ip()
{
  ostringstream st;
  srand(time(0));

//      int num = (rand() % (upper - lower + 1)) + lower

  int num = (rand() % (255 - 0 + 1));

  st << num;
  st << ".";

  num = (rand() % (255 - 0 + 1));
  st << num;
  st << ".";

  num = (rand() % (255 - 0 + 1));
  st << num;
  st << ".";

  num = (rand() % (255 - 0 + 1));
  st << num;

  return st.str();
}


string gen_user_number (size_t len)
{
  ostringstream st;
  srand(time(0));

  for (size_t i = 0; i < len; i++)
      {
       int num = (rand() % (9 - 0 + 1));
       st << num;
      }

  return st.str();
}


string gen_user_word (size_t len)
{
  ostringstream st;
  srand(time(0));

  for (size_t i = 0; i < len; i++)
      {
       int g = std::rand()%25;
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

string get_datetime (const string &format)
{
  auto t = std::time (nullptr);
  auto tm = *std::localtime(&t);

  std::ostringstream oss;
  //oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
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


CTpl::CTpl (const string &fname): CPairFile (fname, false)
{

//cout << "gen_random_ip():" << gen_random_ip() << endl;

}


string CTpl::prepare_log_string()
{
  string logstring = get_string ("LOGSTRING", "IP - USER [DATETIME +0000] \"REQUEST / URI PROTOCOL\" STATUS BYTES \" STATIC_TEXT \" ");

  string ip = gen_random_ip();

  logstring.replace (logstring.find("IP"), string("IP").size(), ip);

  string user = get_string ("USER", "WORD|NUMBER");

  if (user == "NUMBER")
     logstring.replace(logstring.find("USER"), string("USER").size(), gen_user_number(8));
  else
  if (user == "WORD")
     logstring.replace(logstring.find("USER"), string("USER").size(), gen_user_word(8));
  else
      {
       //get random
       srand(time(0));
       int num = rand() % 2;
       if (num == 0)
          logstring.replace(logstring.find("USER"), string("USER").size(), gen_user_number(8));
       else
            logstring.replace(logstring.find("USER"), string("USER").size(), gen_user_word(8));
      }

  string datetime = get_string ("DATETIME", "%x:%X");
  logstring.replace(logstring.find("DATETIME"), string("DATETIME").size(), get_datetime (datetime));
  //add msecs support

  //string request = get_string ("USER", "WORD");


  return logstring;
};
