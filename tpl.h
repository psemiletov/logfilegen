/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#ifndef TPL_H
#define TPL_H

#include <string>
#include <vector>
#include <map>
#include <random>


#include "libretta_pairfile.h"

using namespace std;


class CTpl: public CPairFile
{
public:

  std::random_device rnd_dev;
  std::mt19937 *rnd_generator;

  map <string, string> logstrings;

  CPairFile *templatefile;

  string mode; //nginx, apache, etc

  string tlogstring;
  string ip;
  string user;
  string datetime;

  string request;
  vector <string> v_request;


  string uri;
  string protocol;

  string status;
  vector <string> v_status;

  string body_bytes_sent;
  vector <string> v_body_bytes_sent;


  string http_referer;
  string http_user_agent;

  CTpl (const string &fname, const string &amode);
  ~CTpl();

  int get_rnd (int a, int b);


  string gen_random_ip();
  string gen_user_number (size_t len);
  string gen_user_word (size_t len);
  string get_datetime (const string &format);




  string prepare_log_string();
};


#endif

