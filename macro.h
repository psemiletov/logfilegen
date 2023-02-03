#ifndef MACRO_H
#define MACRO_H

#include <string>
#include <vector>
#include <random>
#include <map>


using namespace std;


//class CMacrosPoolMeta;

class CMacro
{
public:
   std::mt19937 *rnd_generator;
   std::random_device rnd_dev;

   string text;
   string meta; //состоит из @1 @2 ..

   size_t len_min;
   size_t len_max;
   size_t length;

   CMacro();
   virtual ~CMacro();
   virtual CMacro* create_self (const string &s) {return new CMacro();};
   virtual string process() {return string ("CMacro::process()");};
   virtual void parse (const string &s) {};
   virtual void interpet (const string &s) {};

};


class CMacroIPRandom: public CMacro
{
public:
   CMacroIPRandom* create_self (const string &s);
   string process();
   void parse (const string &s){};

   ~CMacroIPRandom(){};
};


class CMacroStrRandom: public CMacro
{
public:

  CMacroStrRandom* create_self (const string &s);
  string process();
  void parse (const string &s);

  ~CMacroStrRandom(){};
};


class CMacroIntRandom: public CMacro
{
public:

  CMacroIntRandom* create_self (const string &s);
  string process();
  void parse (const string &s);
  ~CMacroIntRandom(){};
};


class CMacroHexRandom: public CMacro
{
public:

  CMacroHexRandom* create_self (const string &s);
  string process();
  void parse (const string &s);
  ~CMacroHexRandom(){};
};


class CMacroDateTime: public CMacro
{
public:

  CMacroDateTime* create_self (const string &s);
  string process();
  void parse (const string &s);
  ~CMacroDateTime(){};
};


class CMacroPathRandom: public CMacro
{
public:

  CMacroPathRandom* create_self (const string &s);
  string process();
  void parse (const string &s);
  ~CMacroPathRandom(){};
};


class CMacroSeq: public CMacro
{
public:
  vector <string> vt;

  CMacroSeq* create_self (const string &s);
  string process();
  void parse (const string &s);
  ~CMacroSeq(){};
};


class CMacroFileSource: public CMacro
{
public:

  vector <string> vt;

  CMacroFileSource* create_self (const string &s);
  string process();
  void parse (const string &s);
  ~CMacroFileSource(){};
};





class CMacrosPool
{
public:

   map <string, CMacro*> macros;

   CMacrosPool();
   ~CMacrosPool();
};


class CMacrosPoolMeta
{
public:

   map <string, CMacro*> macros;

   CMacrosPoolMeta();
   ~CMacrosPoolMeta();
};


class CMacrosCache
{
public:

   map <int, CMacro*> macros;

   void add (size_t pos, CMacro *m);
   ~CMacrosCache();
};


class CMacrosCacheMeta
{
public:

   map <string, CMacro*> macros;

   void add (const string &s, CMacro *m);
   ~CMacrosCacheMeta();
};



class CMacroMeta: public CMacro
{
public:

  CMacrosPoolMeta pool;
  CMacrosCacheMeta cache;

  vector <string> vt;

  CMacroMeta* create_self (const string &s);
  string process();
  void parse (const string &s);
  ~CMacroMeta(){};
};


#endif
