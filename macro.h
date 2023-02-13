#ifndef MACRO_H
#define MACRO_H

#include <string>
#include <vector>
#include <random>
#include <map>


//using namespace std;


//class CMacrosPoolMeta;

class CMacro
{
public:
   std::mt19937 *rnd_generator;
   std::random_device rnd_dev;

   std::string text;
   std::string meta; //состоит из @1 @2 ..

   size_t len_min;
   size_t len_max;
   size_t length;

   CMacro();
   virtual ~CMacro();
   virtual CMacro* create_self (const std::string &s) {return new CMacro();};
   virtual std::string process() {return std::string ("CMacro::process()");};
   virtual void parse (const std::string &s) {};
   virtual void interpet (const std::string &s) {};

};


class CMacroIPRandom: public CMacro
{
public:
   CMacroIPRandom* create_self (const std::string &s);
   std::string process();
   void parse (const std::string &s){};

   ~CMacroIPRandom(){};
};


class CMacroStrRandom: public CMacro
{
public:

  CMacroStrRandom* create_self (const std::string &s);
  std::string process();
  void parse (const std::string &s);

  ~CMacroStrRandom(){};
};


class CMacroIntRandom: public CMacro
{
public:

  CMacroIntRandom* create_self (const std::string &s);
  std::string process();
  void parse (const std::string &s);
  ~CMacroIntRandom(){};
};


class CMacroHexRandom: public CMacro
{
public:

  CMacroHexRandom* create_self (const std::string &s);
  std::string process();
  void parse (const std::string &s);
  ~CMacroHexRandom(){};
};


class CMacroDateTime: public CMacro
{
public:

  CMacroDateTime* create_self (const std::string &s);
  std::string process();
  void parse (const std::string &s);
  ~CMacroDateTime(){};
};


class CMacroPathRandom: public CMacro
{
public:

  CMacroPathRandom* create_self (const std::string &s);
  std::string process();
  void parse (const std::string &s);
  ~CMacroPathRandom(){};
};


class CMacroSeq: public CMacro
{
public:
  std::vector <std::string> vt;

  CMacroSeq* create_self (const std::string &s);
  std::string process();
  void parse (const std::string &s);
  ~CMacroSeq(){};
};


class CMacroFileSource: public CMacro
{
public:

  std::vector <std::string> vt;

  CMacroFileSource* create_self (const std::string &s);
  std::string process();
  void parse (const std::string &s);
  ~CMacroFileSource(){};
};



class CMacrosPool
{
public:

   std::map <std::string, CMacro*> macros;

   CMacrosPool();
   ~CMacrosPool();
};


class CMacrosPoolMeta
{
public:

   std::map <std::string, CMacro*> macros;

   CMacrosPoolMeta();
   ~CMacrosPoolMeta();
};


class CMacrosCache
{
public:

   std::map <int, CMacro*> macros;

   void add (size_t pos, CMacro *m);
   ~CMacrosCache();
};


class CMacrosCacheMeta
{
public:

   std::map <std::string, CMacro*> macros;

   void add (const std::string &s, CMacro *m);
   ~CMacrosCacheMeta();
};



class CMacroMeta: public CMacro
{
public:

  CMacrosPoolMeta pool;
  CMacrosCacheMeta cache;

  std::vector <std::string> vt;

  CMacroMeta* create_self (const std::string &s);
  std::string process();
  void parse (const std::string &s);
  ~CMacroMeta(){};
};


#endif
