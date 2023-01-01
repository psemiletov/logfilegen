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

  string status;

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

