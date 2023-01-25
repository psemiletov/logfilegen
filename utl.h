/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#ifndef UTL_H
#define UTL_H

#include <string>
#include <vector>

using namespace std;

//File utils

size_t get_file_size (const string &fname);
string get_file_ext (const string &fname);
string replace_file_ext (const string &fname, const string &ext);
string get_file_path (const string &path);
string get_home_dir();
string current_path();
size_t get_free_space (const string &path);
bool file_exists (const string &name);
bool is_program_exists (const string &appname);
bool is_path_abs (const string &path);


//String utis

string str_replace (string &source, const string &text_to_find, const string &replace_with);
string string_replace_all (const string &s, const string &from, const string &to);
vector <string> split_string_to_vector (const string& s, const string& delimeter, const bool keep_empty = true);
string string_to_lower (const string &s);
size_t string_to_file_size (const string &val);
string string_file_load (const string &fname);
vector <string> vector_file_load (const string &fname);



#endif
