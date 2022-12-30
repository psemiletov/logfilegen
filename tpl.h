/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#ifndef TPL_H
#define TPL_H

#include <string>
#include <vector>
#include <map>
#include <random>


#include "libretta_pairfile.h"

using namespace std;


class CTpl: public CPairFile
{
public:

  std::random_device rnd_dev;
  std::mt19937 *rnd_generator;

  CPairFile *templatefile;

  string tlogstring;
  string logstring;
  string ip;
  string user;
  string datetime;

  CTpl (const string &fname);
  ~CTpl();

  string gen_random_ip();


  string prepare_log_string();
};


#endif

