/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#ifndef TPL_H
#define TPL_H

#include <string>
#include <vector>
#include <map>
#include <random>

#include "pairfile.h"


using namespace std;

#define VT_NONE 0
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
  int precision;
  int rnd_length;

  int rnd_path_deep;
  int rnd_path_min;
  int rnd_path_max;

  int a;
  int b;

  double fa;
  double fb;

  string k; //key
  vector <string> v; //values

  CVar (const string &key, const string &val);
  ~CVar();

  int get_rnd (int ta, int tb);

  string gen_random_ip();
  string gen_number (size_t len);
  string gen_word (size_t len);
  //string gen_word_range (size_t min, size_t max);

  string gen_msecs();
  string gen_rnd_path (size_t min, size_t max, size_t deep);


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
  void replace_value_by_key (const string &key, const string &value);

};


#endif

