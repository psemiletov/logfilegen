/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#ifndef UTL_H
#define UTL_H

#include <string>
#include <vector>

using namespace std;

//File utils

string get_file_ext (const string &fname);
string replace_file_ext (const string &fname, const string &ext);
string increase_file_ext (const string &fname);



string get_file_path (const string &path);
string get_home_dir();
string current_path();
bool file_exists (const string &name);
size_t get_free_space (const string &path);

//String utis

string str_replace (string &source, const string &text_to_find, const string &replace_with);
vector <string> split_string_to_vector (const string& s, const string& delimeter, const bool keep_empty = true);
size_t string_to_file_size (const string &val);



#endif
