#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <ctime>
#include <chrono>


#ifdef USE_PROM
#include "prometheus/client_metric.h"
#include "prometheus/counter.h"
#include "prometheus/exposer.h"
#include "prometheus/family.h"
#include "prometheus/info.h"
#include "prometheus/registry.h"


#endif


#include "utl.h"
#include "macro.h"

using namespace std;


std::mt19937 &mt()
{
  // initialize once per thread
  //thread_local static std::random_device srd;
  //thread_local static std::mt19937 smt(srd());


  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  thread_local static std::mt19937 smt(seed);

  return smt;
}



char arr_nums [] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};


int get_rnd (std::mt19937 *rnd_generator, int ta, int tb)
{
  std::uniform_int_distribution <> distrib (ta, tb);
  return distrib (*rnd_generator);
}


string gen_string (std::mt19937 *rnd_generator, size_t len)
{
  string result;
  result.reserve (128);

  std::uniform_int_distribution<> distrib (0, 25);

  for (size_t i = 0; i < len; i++)
      {
       int g = distrib (*rnd_generator);
       char d = static_cast<char> (g + 'a');
       result += d;
      }

  return result;
}


string gen_string (std::mt19937 *rnd_generator, size_t min, size_t max)
{
  string result;
  result.reserve (128);

  std::uniform_int_distribution<> distrib (0, 25);
  std::uniform_int_distribution<> dminmax (min, max);
  size_t len = dminmax (*rnd_generator);

  for (size_t i = 0; i < len; i++)
      {
       int g = distrib (*rnd_generator);
       char d = static_cast<char> (g + 'a');
       result += d;
      }

  return result;
}


string gen_rnd_path (std::mt19937 *rnd_generator, size_t min, size_t max, size_t deep)
{
  string result;
  result.reserve (256);

  size_t deep_max = get_rnd (rnd_generator, 1, deep);

  for (size_t d = 0; d < deep_max; d++)
      {
       result += "/";
       int len = get_rnd (rnd_generator, min, max);

       result += gen_string (rnd_generator, len);
      }

  return result;
}


string gen_number (std::mt19937 *rnd_generator, size_t len)
{
  std::uniform_int_distribution<> distrib (0, 9);

  string result;
  result.reserve (len);

  for (size_t i = 0; i < len; i++)
      {
       result += arr_nums[distrib (*rnd_generator)];
      }

  return result;
}


string gen_number (std::mt19937 *rnd_generator, size_t min, size_t max)
{
  std::uniform_int_distribution<> distrib (0, 9);
  std::uniform_int_distribution<> dminmax (min, max);
  size_t len = dminmax (*rnd_generator);

  string result;
  result.reserve (len);

  for (size_t i = 0; i < len; i++)
      {
       result += arr_nums[distrib (*rnd_generator)];
      }

  return result;
}


string gen_hex_number (std::mt19937 *rnd_generator, size_t len)
{
  std::uniform_int_distribution<> distrib (0, 15);

  string result;
  result.reserve (len);

  for (size_t i = 0; i < len; i++)
      {
       result += arr_nums[distrib (*rnd_generator)];
      }

  return result;
}


string gen_hex_number (std::mt19937 *rnd_generator, size_t min, size_t max)
{
  std::uniform_int_distribution<> distrib (0, 15);
  std::uniform_int_distribution<> dminmax (min, max);
  size_t len = dminmax (*rnd_generator);

  string result;
  result.reserve (len);

  for (size_t i = 0; i < len; i++)
      {
       result += arr_nums[distrib (*rnd_generator)];
      }

  return result;
}


CMacro::CMacro()
{
  rnd_generator = new std::mt19937 (rnd_dev());

  len_min = 0;
  len_max = 0;
  length = 0;
}


CMacro::~CMacro()
{
  delete rnd_generator;
}


CMacrosPool::CMacrosPool()
{
  macros.insert (std::make_pair ("@ip", new CMacroIPRandom()));
  macros.insert (std::make_pair ("@str", new CMacroStrRandom()));
  macros.insert (std::make_pair ("@int", new CMacroIntRandom()));
  macros.insert (std::make_pair ("@hex", new CMacroHexRandom()));
  macros.insert (std::make_pair ("@datetime", new CMacroDateTime()));
  macros.insert (std::make_pair ("@path", new CMacroPathRandom()));
  macros.insert (std::make_pair ("@file", new CMacroFileSource()));
  macros.insert (std::make_pair ("@meta", new CMacroMeta()));
  macros.insert (std::make_pair ("@seq", new CMacroSeq()));
}


CMacrosPool::~CMacrosPool()
{
  for (auto itr = macros.begin(); itr != macros.end(); ++itr)
      {
       delete (itr->second);
      }
}


CMacrosPoolMeta::CMacrosPoolMeta()
{
  macros.insert (std::make_pair ("@ip", new CMacroIPRandom()));
  macros.insert (std::make_pair ("@str", new CMacroStrRandom()));
  macros.insert (std::make_pair ("@int", new CMacroIntRandom()));
  macros.insert (std::make_pair ("@hex", new CMacroHexRandom()));
  macros.insert (std::make_pair ("@datetime", new CMacroDateTime()));
  macros.insert (std::make_pair ("@path", new CMacroPathRandom()));
  macros.insert (std::make_pair ("@file", new CMacroFileSource()));
  macros.insert (std::make_pair ("@seq", new CMacroSeq()));
}


CMacrosPoolMeta::~CMacrosPoolMeta()
{
  for (auto itr = macros.begin(); itr != macros.end(); ++itr)
      {
       delete (itr->second);
      }
}


CMacroIPRandom* CMacroIPRandom::create_self (const string &s)
{
  return new CMacroIPRandom();
}


string CMacroIPRandom::process()
{
  std::uniform_int_distribution<> distrib (0, 255);

  string result;
  result.reserve (16);

  result += to_string (distrib (*rnd_generator));
  result += ".";

  result += to_string (distrib (*rnd_generator));
  result += ".";

  result += to_string (distrib (*rnd_generator));
  result += ".";

  result += to_string (distrib (*rnd_generator));

  return result;
}


CMacroStrRandom* CMacroStrRandom::create_self (const string &s)
{
  CMacroStrRandom *m = new CMacroStrRandom();
  m->parse (s);
  return m;
}


void CMacroStrRandom::parse (const string &s)
{
  len_min = 0;
  len_max = 0;
  length = 0;

  vector <string> vt = split_string_to_vector (s, ":");

  if (vt.size() == 2)
      length = atoi (vt[1].c_str());

  if (vt.size() == 3)
      {
       len_min = atoi (vt[1].c_str());
       len_max = atoi (vt[2].c_str());
      }
}


string CMacroStrRandom::process()
{
  if (len_max == 0)
     return gen_string (rnd_generator, length);

  return gen_string (rnd_generator, len_min, len_max);
}


CMacroIntRandom* CMacroIntRandom::create_self (const string &s)
{
  CMacroIntRandom *m = new CMacroIntRandom();
  m->parse (s);
  return m;
}


void CMacroIntRandom::parse (const string &s)
{
  len_min = 0;
  len_max = 0;
  length = 0;

  vector <string> vt = split_string_to_vector (s, ":");

  if (vt.size() == 2)
      length = atoi (vt[1].c_str());

  if (vt.size() == 3)
      {
       len_min = atoi (vt[1].c_str());
       len_max = atoi (vt[2].c_str());
      }
}


string CMacroIntRandom::process()
{
  if (len_max == 0)
     return gen_number (rnd_generator, length);

  return gen_number (rnd_generator, len_min, len_max);
}


CMacroHexRandom* CMacroHexRandom::create_self (const string &s)
{
  CMacroHexRandom *m = new CMacroHexRandom();
  m->parse (s);
  return m;
}


void CMacroHexRandom::parse (const string &s)
{
  len_min = 0;
  len_max = 0;
  length = 0;

  vector <string> vt = split_string_to_vector (s, ":");

  if (vt.size() == 2)
      length = atoi (vt[1].c_str());

  if (vt.size() == 3)
      {
       len_min = atoi (vt[1].c_str());
       len_max = atoi (vt[2].c_str());
      }
}


string CMacroHexRandom::process()
{
  if (len_max == 0)
     return gen_hex_number (rnd_generator, length);

  return gen_hex_number (rnd_generator, len_min, len_max);
}


CMacroDateTime* CMacroDateTime::create_self (const string &s)
{
  CMacroDateTime *m = new CMacroDateTime();
  m->parse (s);
  return m;
}


void CMacroDateTime::parse (const string &s)
{
  len_min = 0;
  len_max = 0;
  length = 0;
  text = "";

  size_t pos = s.find (":");
  if (pos != string::npos)
     text = s.substr (pos + 1);

}

string CMacroDateTime::process()
{
  if (text.empty())
     return string();

  time_t rawtime;
  struct tm * timeinfo;

  time (&rawtime);
  timeinfo = localtime (&rawtime);

  char buffer [128];
  strftime (buffer, 128, text.c_str(), timeinfo);

  auto result = buffer;
  return result;
}


CMacroPathRandom* CMacroPathRandom::create_self (const string &s)
{
  CMacroPathRandom *m = new CMacroPathRandom();
  m->parse (s);
  return m;
}


void CMacroPathRandom::parse (const string &s)
{
  len_min = 0;
  len_max = 0;
  length = 0;
  text = "";

  vector <string> vt = split_string_to_vector (s, ":");
  if (vt.size() == 4)
     {
     len_min = atoi (vt[1].c_str());
     len_max = atoi (vt[2].c_str());
     length = atoi (vt[3].c_str());

     if (length == 0)
         length = 1;
    }

}


string CMacroPathRandom::process()
{
   return gen_rnd_path (rnd_generator, len_min, len_max, length);
}


CMacroSeq* CMacroSeq::create_self (const string &s)
{
  CMacroSeq *m = new CMacroSeq();
  m->parse (s);
  return m;
}


vector <string> split_string_to_vector_a (const string &s, char delimeter, char aware)
{
  vector <string> result;

  size_t a = 0;
  size_t b = 0;

  while (b < s.size())
        {
         if (b + 1 == s.size())
            {
             string t = s.substr (a + 1, b - a);
             result.push_back (t);
            }

         if (s[b] == delimeter)
             if (b + 1 != s.size())
                if (s[b + 1] != aware)
                   {
                    string t;

                    if (a == 0)
                        t = s.substr (a, b - a);
                    else
                        t = s.substr (a + 1, b - a - 1);

                    result.push_back (t);
                    a = b;
                   }

         b++;
        }

  return result;
}


void CMacroSeq::parse (const string &s)
{
  len_min = 0;
  len_max = 0;
  length = 0;
  text = "";

  vt = split_string_to_vector_a (s, ':', '/');
}


string CMacroSeq::process()
{
   size_t i = get_rnd (rnd_generator, 1, vt.size() - 1);

   return vt[i];
}


CMacroFileSource* CMacroFileSource::create_self (const string &s)
{
  CMacroFileSource *m = new CMacroFileSource();
  m->parse (s);
  return m;
}


void CMacroFileSource::parse (const string &s)
{
  len_min = 0;
  len_max = 0;
  length = 0;
  text = "";

  size_t pos = s.find (":");
  if (pos == string::npos)
      return;

  string path = s.substr (pos + 1);
  vt = vector_file_load (path);
}


string CMacroFileSource::process()
{
  if (vt.size() != 0)
     text = vt[get_rnd (rnd_generator, 0, vt.size()-1)];

  return text;
}


CMacroMeta* CMacroMeta::create_self (const string &s)
{
  CMacroMeta *m = new CMacroMeta();
  m->parse (s);
  return m;
}


void CMacroMeta::parse (const string &s)
{
  len_min = 0;
  len_max = 0;
  length = 0;
  text = "";

  //отделяем название макроса
  size_t pos = s.find (":");
  if (pos != string::npos)
     text = s.substr (pos + 1); //after :
  else
      return; //check it

  // cout << "all text of macro: " << text << endl;

  //ищем вложенные макросы

  size_t i = 0;

  while (i < text.size())
        {
         if (text[i] == '(') //start of macro
            {
       //      cout << "//start of macro" << endl;

             //search to ), then get macro text
             size_t j = i;

             bool found = false;

             while (j < text.size())
                   {
                    if (text[j] == ')')
                       {
                        found = true;
                        break;
                       }

                    j++;
                   }


           //now, macro text in between i and j
           if (found)
              {
               string macrotext = text.substr (i, j-i + 1);
               i += macrotext.size();

               macrotext.pop_back();
               macrotext.erase (0, 1);

   //            cout << "macrotext:" << macrotext << endl;

               //create cached macro
               string name = get_macro_name (macrotext);

     //          cout << "and name is: " << name << endl;

               if (name.empty())
                  continue;

               auto f = pool.macros.find (name);
               if (f == pool.macros.end())
                  continue;

               //copy metamacro instead of real one
               string newname = "@" + to_string(i);

               meta += newname;

               CMacro *tm = f->second->create_self (macrotext);

               cache.add (newname, tm);
              }
           }

         meta += text[i];
        i++;
       }
}


string CMacroMeta::process()
{
  text = meta;

  map <string, CMacro*>::iterator it;
  for (it = cache.macros.begin(); it != cache.macros.end(); it++)
      {
       string macroname = it->first; //@1, @2, etc
   //   cout << "macroname:" << macroname << endl;
       size_t i = 0;
       do
         {
          i = text.find (macroname);
          if (i != string::npos)
              text.replace (i, macroname.length(), it->second->process());
          }
       while (i != string::npos);
      }

 // cout << "text:"  << text << endl;
  return text;
}

void CMacrosCache::add (size_t pos, CMacro *m)
{
  macros.insert (std::make_pair (pos, m));
}


CMacrosCache::~CMacrosCache()
{
  for (auto itr = macros.begin(); itr != macros.end(); ++itr)
      {
       delete (itr->second);
      }
}


void CMacrosCacheMeta::add (const string &s, CMacro *m)
{
  macros.insert (std::make_pair (s, m));
}


CMacrosCacheMeta::~CMacrosCacheMeta()
{
  for (auto itr = macros.begin(); itr != macros.end(); ++itr)
      {
       delete (itr->second);
      }
}
