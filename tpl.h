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


class CTpl
{
public:

  CPairFile *pf;

  map <string, CVar*> vars;
  map <string, string> logstrings;

  string mode; //nginx, apache, etc

  CTpl (const string &fname, const string &amode);
  ~CTpl();

  string prepare_log_string();
};


#endif

