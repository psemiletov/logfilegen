/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#ifndef TPL_H
#define TPL_H

#include <string>
#include <vector>
#include <map>
#include <random>
#include <memory>


#include "libretta_pairfile.h"

using namespace std;


#define VT_SINGLE 1
#define VT_SEQ 2
#define VT_RANGE 3
#define VT_FLOATRANGE 4


class CVar
{
public:

  std::mt19937 *rnd_generator;
  std::random_device rnd_dev;

  int vartype;

  int rnd_length;
  int a;
  int b;

  double fa;
  double fb;

  int precision;


  string k; //key
  vector <string> v; //values

  CVar (const string &key, const string &val);
  ~CVar();

  int get_rnd (int ta, int tb);


  string gen_random_ip();
  string gen_number (size_t len);
  string gen_word (size_t len);

  string gen_msecs();

  string get_datetime (const string &format);

  string get_val();
/*
  bool operator > (const CVar &v) const
      {
        return (k > k.k);
      }
*/
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



class CTpl2
{
public:

  CPairFile *pf;

  vector <CVar*> vars;

  map <string, string> logstrings;

  string mode; //nginx, apache, etc

  CTpl2 (const string &fname, const string &amode);
  ~CTpl2();

  string prepare_log_string();
};


#endif

