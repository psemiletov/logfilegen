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

/*

 IP={{ RANDOM IP }}
USER={{ WORD | NUMBER }}
TIME={{dd/mm/yyyy:hh:mm:ss z}}
REQUEST=["GET", "POST"]
URI=["api", "docs"]
PROTOCOL=["HTTP/1.1", "HTTP/2.0"]
STATUS=[200, 403]
BYTES=[0-5000]
STATIC_TEXT="static text"




 {{ IP }} - {{ USER }} [{{ TIME }}] "{{ REQUEST }} /{{ URI }} {{ PROTOCOL }}" {{ STATUS }} {{ BYTES }} "{{ STATIC_TEXT }}"


 209.124.125.232 - abcd1234 [22/Dec/2022:18:56:54 +0000] "GET /api HTTP/1.1" 403 1234 "static text"
209.124.125.132 - 5678efgi [22/Dec/2022:18:56:55 +0000] "GET /docs HTTP/2.0" 200 5678 "static text"

 */

class CTpl: public CPairFile
{
public:

//read the template from file to following variables
/*
  string ip;
  string user;
  string time; //time format
  string request;

  string template_string;
*/

// CTpl (const string &fname, bool from_data = false);
  CTpl (const string &fname);


  string prepare_log_string();
};


#endif

