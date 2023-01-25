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


   string name;
   string text;

   size_t len_min;
   size_t len_max;
   size_t length;

   CMacro();
   virtual ~CMacro();
   virtual CMacro* create_self (const string &s) {return new CMacro();};
   virtual string process() {return string ("CMacro::process()");};
   virtual void parse (const string &s) {};

};


class CMacroIPRandom: public CMacro
{
public:
   //string test;

   CMacroIPRandom* create_self (const string &s);
   string process();
   void parse (const string &s){};
   CMacroIPRandom();
   ~CMacroIPRandom(){};
};


class CMacroStrRandom: public CMacro
{
public:

  CMacroStrRandom* create_self (const string &s);
  string process();
  void parse (const string &s);

  CMacroStrRandom();
  ~CMacroStrRandom(){};
};


class CMacroIntRandom: public CMacro
{
public:

  CMacroIntRandom* create_self (const string &s);
  string process();
  void parse (const string &s);
  CMacroIntRandom();
  ~CMacroIntRandom(){};
};


class CMacroDateTime: public CMacro
{
public:

  CMacroDateTime* create_self (const string &s);
  string process();
  void parse (const string &s);
  CMacroDateTime();
  ~CMacroDateTime(){};
};


class CMacroPathRandom: public CMacro
{
public:

  CMacroPathRandom* create_self (const string &s);
  string process();
  void parse (const string &s);
  CMacroPathRandom();
  ~CMacroPathRandom(){};
};



class CMacroFileSource: public CMacro
{
public:

  vector <string> vt;

  CMacroFileSource* create_self (const string &s);
  string process();
  void parse (const string &s);
  CMacroFileSource();
  ~CMacroFileSource(){};
};



class CMacrosPool
{
public:

   vector <CMacro*> macros;

   CMacro* find (const string &name);

   CMacrosPool();
   ~CMacrosPool();
};


#endif
