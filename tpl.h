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

  std::map <std::string, CVar*> vars;
  std::map <std::string, std::string> logstrings;

  std::string mode; //nginx, apache, etc

  CTpl (const std::string &fname, const std::string &amode);
  ~CTpl();

  std::string prepare_log_string();
  void replace_value_by_key (const std::string &key, const std::string &value);

};

#endif
