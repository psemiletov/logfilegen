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


#define VT_SINGLE 1
#define VT_SEQ 2
#define VT_RANGE 3


class CTpl: public CPairFile
{
public:

  std::random_device rnd_dev;
  std::mt19937 *rnd_generator;

  map <string, string> logstrings;


  string mode; //nginx, apache, etc

  string tlogstring;
  string ip;
  string user;
  string datetime;

  string request;
  vector <string> v_request;


  string uri;
  vector <string> v_uri;

  string protocol;
  vector <string> v_protocol;

  string status;
  vector <string> v_status;

  string body_bytes_sent;
  vector <string> v_body_bytes_sent;


  string http_referer;

  string http_user_agent;
  vector <string> v_http_user_agent;



  CTpl (const string &fname, const string &amode);
  ~CTpl();

  int get_rnd (int a, int b);


  string gen_random_ip();
  string gen_user_number (size_t len);
  string gen_user_word (size_t len);
  string get_datetime (const string &format);




  string prepare_log_string();
};



class CVar
{
public:

  std::mt19937 *rnd_generator;
  std::random_device rnd_dev;

  int vartype;

  int a;
  int b;
  vector <string> v;

  CVar (const string &val);
  ~CVar();

  int get_rnd (int ta, int tb);

  string gen_random_ip();
  string gen_number (size_t len);
  string gen_word (size_t len);
  string get_datetime (const string &format);


  string get_val();

};


class CTpl2
{
public:

  CPairFile *pf;

  //map <string, CVar*> vars;

map <string, CVar*> vars;



  map <string, string> logstrings;

  //  std::random_device rnd_dev;
//  std::mt19937 *rnd_generator;


  string mode; //nginx, apache, etc



  CTpl2 (const string &fname, const string &amode);
  ~CTpl2();

  //int get_rnd (int a, int b);

/*
  string gen_random_ip();
  string gen_user_number (size_t len);
  string gen_user_word (size_t len);
  string get_datetime (const string &format);
*/
  string prepare_log_string();
};


#endif

