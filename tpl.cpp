#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <algorithm>

#include "tpl.h"


/*

 IP={{ RANDOM IP }}
USER={{ WORD | NUMBER }}
TIME={{dd/mm/yyyy:hh:mm:ss z}}
REQUEST=["GET", "POST"]
URI=["api", "docs"]
PROTOCOL=["HTTP/1.1", "HTTP/2.0"]
STATUS=[200, 403]
BYTES=[0-5000]
STATIC_TEXT="static text"



 {{ IP }} - {{ USER }} [{{ TIME }}] "{{ REQUEST }} /{{ URI }} {{ PROTOCOL }}" {{ STATUS }} {{ BYTES }} "{{ STATIC_TEXT }}"


 209.124.125.232 - abcd1234 [22/Dec/2022:18:56:54 +0000] "GET /api HTTP/1.1" 403 1234 "static text"
209.124.125.132 - 5678efgi [22/Dec/2022:18:56:55 +0000] "GET /docs HTTP/2.0" 200 5678 "static text"

 */


string gen_random_ip()
{
  ostringstream st;
  srand(time(0));

//      int num = (rand() % (upper - lower + 1)) + lower

  int num = (rand() % (255 - 0 + 1)) + 0;
  st << num;
  st << ".";

  num = (rand() % (255 - 0 + 1)) + 0;
  st << num;
  st << ".";

  num = (rand() % (255 - 0 + 1)) + 0;
  st << num;
  st << ".";

  num = (rand() % (255 - 0 + 1)) + 0;
  st << num;

  return st.str();
}


string gen_user_number (size_t len)
{
  ostringstream st;
  srand(time(0));

  for (size_t i = 0; i < len; i++)
      {
       int num = (rand() % (9 - 0 + 1)) + 0;
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


CTpl::CTpl (const string &fname): CPairFile (fname, false)
{

//cout << "gen_random_ip():" << gen_random_ip() << endl;

}


string CTpl::prepare_log_string()
{
  string logstring = get_string ("LOGSTRING", "IP - USER [TIME] \"REQUEST / URI PROTOCOL\" STATUS BYTES \" STATIC_TEXT \" ");

  string ip = gen_random_ip();

  logstring.replace(logstring.find("IP"), string("IP").size(), ip);

  string user = get_string ("USER", "WORD");

  if (user == "NUMBER")
     logstring.replace(logstring.find("USER"), string("USER").size(), gen_user_number(8));

  if (user == "WORD")
     logstring.replace(logstring.find("USER"), string("USER").size(), gen_user_word(8));



  return logstring;
};
