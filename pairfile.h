/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#ifndef PAIRFILE_H
#define PAIRFILE_H

#include <string>
#include <vector>
#include <map>

using namespace std;

typedef std::map <string, string> TSPair;

class CPairFile
{
public:
  
  string file_name;
  
  TSPair values;

  bool get_bool (const string &key, bool def_value = false);
  int get_int (const string &key, int def_value = 0);
//  unsigned int get_uint (const string &key, unsigned int def_value = 0);
  size_t get_num (const string &key, unsigned long long def_value = 0);

  float get_float (const string &key, float def_value = 0.0f);
  string get_string (const string &key, const string &def_value = "");
  size_t get_file_size (const string &key, const string &def_value = "64k"); //return value in bytes

  void set_string (const string &key, const string &value);
  void set_int (const string &key, int value);
  void set_float (const string &key, float value);
  
  void save();
  
  CPairFile (const string &fname, bool from_data = false);
  CPairFile (int argc, char *argv[]);
  CPairFile (vector <string> envars); //ENV VARS

};

#endif
