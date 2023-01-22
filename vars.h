/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#ifndef VARS_H
#define VARS_H

#include <string>
#include <vector>
#include <random>



using namespace std;

#define VT_NONE 0
#define VT_SINGLE 1
#define VT_SEQ 2
#define VT_RANGE 3
#define VT_FLOATRANGE 4
#define VT_DATETIME 5


class CVar
{
public:

  std::mt19937 *rnd_generator;
  std::random_device rnd_dev;

  int vartype;
  int precision;
  int rnd_length;

  size_t rnd_path_deep;
  size_t rnd_path_min;
  size_t rnd_path_max;

  int len_min;
  int len_max;

  int a;
  int b;

  double fa;
  double fb;

  string k; //key
  vector <string> v; //values

  CVar (const string &key, const string &val);
  ~CVar();

  int get_rnd (int ta, int tb);
  string gen_rnd_path (size_t min, size_t max, size_t deep);
  string gen_msecs();
  string gen_random_ip();
  string gen_number (size_t len);
  string gen_number (size_t min, size_t max);
  string gen_word (size_t len);
  string gen_string (size_t min, size_t max);
  string get_datetime (const string &format);
  string get_val();

};



#endif
