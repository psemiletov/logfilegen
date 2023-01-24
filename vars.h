/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#ifndef VARS_H
#define VARS_H

#include <string>
#include <vector>
#include <random>

#include "macro.h"


using namespace std;

#define VT_NONE 0
#define VT_SINGLE 1
#define VT_SEQ 2
#define VT_RANGE 3
#define VT_FLOATRANGE 4
#define VT_DATETIME 5
#define VT_MACRO 6


class CVar
{
public:

  std::mt19937 *rnd_generator;
  std::random_device rnd_dev;

  CMacrosPool pool;
  map <string, CMacro*> macros;


  int vartype;
  int precision;
  int rnd_length;
/*
  size_t rnd_path_deep;
  size_t rnd_path_min;
  size_t rnd_path_max;

  int len_min;
  int len_max;
*/
  int a;
  int b;

  double fa;
  double fb;

  string k; //key
  vector <string> v; //values

  string macroname;

  CVar (const string &key, const string &val);
  ~CVar();

  int get_rnd (int ta, int tb);
  string gen_msecs();
  string get_val();

};



#endif
