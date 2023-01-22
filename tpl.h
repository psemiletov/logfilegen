/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#ifndef TPL_H
#define TPL_H

#include <map>
#include <string>

#include "pairfile.h"
#include "vars.h"

class CTpl
{
public:

  CPairFile *pf;

  map <string, CVar*> vars;
  map <string, string> logstrings;

  string mode; //nginx, apache, etc

  CTpl (const string &fname, const string &amode);
  ~CTpl();

  string prepare_log_string();
  void replace_value_by_key (const string &key, const string &value);

};


#endif

