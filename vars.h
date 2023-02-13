/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#ifndef VARS_H
#define VARS_H

#include <string>
#include <vector>
#include <random>

#include "macro.h"


//using namespace std;

#define VT_NONE 0
#define VT_SINGLE 1
#define VT_SEQ 2
#define VT_RANGE 3
#define VT_FLOATRANGE 4
#define VT_DATETIME 5
#define VT_MACRO 6
#define VT_MACSEQ 6


class CVar
{
public:

  std::mt19937 *rnd_generator;
  std::random_device rnd_dev;

  CMacrosPool pool;
  CMacrosCache cache;

  int vartype;
  int precision;
  int rnd_length;
  int a;
  int b;

  double fa;
  double fb;

  std::string k; //key
  std::vector <std::string> v; //values

  std::string macroname;

  CVar (const std::string &key, const std::string &val);
  ~CVar();

  int get_rnd (int ta, int tb);
  std::string gen_msecs();
  std::string get_val();

};

#endif
