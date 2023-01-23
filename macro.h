#ifndef MACRO_H
#define MACRO_H

#include <string>
#include <vector>
#include <random>
#include <map>


using namespace std;


class CMacro
{
public:
   std::mt19937 *rnd_generator;
   std::random_device rnd_dev;

   size_t len_min;
   size_t len_max;
   size_t rnd_length;

   CMacro();
   virtual ~CMacro();
   virtual CMacro* create_self (const string &s) {return new CMacro();};
   virtual string process() {return string ("CMacro::process()");};

};


class CMacroIPRandom: public CMacro
{
public:
   //string test;

   CMacroIPRandom* create_self (const string &s);
   string process();
   ~CMacroIPRandom(){};
};


class CMacroStrRandom: public CMacro
{
public:

  CMacroStrRandom* create_self (const string &s);
  string process();
  ~CMacroStrRandom(){};

 // string gen_string (size_t len);
 // string gen_string (size_t min, size_t max);
};


class CMacroIntRandom: public CMacro
{
public:

  CMacroIntRandom* create_self (const string &s);
  string process();
  ~CMacroIntRandom(){};

 // string gen_number (size_t len);
 // string gen_number (size_t min, size_t max);
};




class CMacrosPool
{
public:

   map <string, CMacro*> macros;

   CMacrosPool();
   ~CMacrosPool();
};


#endif
