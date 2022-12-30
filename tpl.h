/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#ifndef TPL_H
#define TPL_H

#include <string>
#include <vector>
#include <map>

#include "libretta_pairfile.h"

using namespace std;


class CTpl: public CPairFile
{
public:

//read the template from file to following variables

  CPairFile *templatefile;

  string tlogstring;
  string logstring;
  string ip;
  string user;
  string datetime;

  CTpl (const string &fname);

  string prepare_log_string();
};


#endif

