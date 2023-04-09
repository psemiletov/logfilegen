#include <iostream>

#include "tpl.h"
#include "utl.h"

using namespace std;

//full nginx vars see at
//http://nginx.org/en/docs/varindex.html
//Apache:
//https://httpd.apache.org/docs/current/mod/mod_log_config.html


void CTpl::replace_value_by_key (const string &key, const string &value)
{
  auto f = vars.find (key);
  if (f != vars.end()) //delete if variable pre-defined
     {
      delete f->second;
      vars.erase (f);
      vars.insert (std::make_pair (key, new CVar (key, value)));
     }
}


CTpl::~CTpl()
{
  delete pf;

  for (auto itr = vars.begin(); itr != vars.end(); ++itr)
      {
       delete (itr->second);
      }
}


CTpl::CTpl (const string &fname, const string &amode)
{
  pf = new CPairFile (fname, false);

  mode = amode;

  logstrings["nginx"] = "$remote_addr - $remote_user [$time_local] \"$request $uri $server_protocol\" $status $body_bytes_sent \"$http_referer\" \"$http_user_agent\"";

  //see https://httpd.apache.org/docs/2.4/mod/mod_log_config.html
  logstrings["apache"] = "\"%h %l %u %t \"%r\" %>s %b\"";


  //DEFAULTS

  if (mode == "nginx")
     {
      vars.insert (std::make_pair ("$logstring", new CVar ("$logstring", logstrings["nginx"])));
      vars.insert (std::make_pair ("$body_bytes_sent", new CVar ("$body_bytes_sent", "1..9999")));
      vars.insert (std::make_pair ("$connection_time", new CVar ("$connection_time", "0.001..60.000")));
      vars.insert (std::make_pair ("$document_uri", new CVar ("$document_uri", "/|/favico.ico|/doc")));
      vars.insert (std::make_pair ("$https", new CVar ("$https", "on|")));
      vars.insert (std::make_pair ("$http_referer", new CVar ("$http_referer", "-")));
      vars.insert (std::make_pair ("$http_user_agent", new CVar ("$http_user_agent", "Mozilla|Chrome|Vivaldi|Opera")));
      vars.insert (std::make_pair ("$is_args", new CVar ("$is_args", "?|")));
      vars.insert (std::make_pair ("$pipe", new CVar ("$pipe", "p|.")));
      vars.insert (std::make_pair ("$protocol", new CVar ("$protocol", " TCP/UDP")));
      vars.insert (std::make_pair ("$remote_addr", new CVar ("$remote_addr", "@ip")));
      vars.insert (std::make_pair ("$remote_user", new CVar ("$remote_user", "@str:8|@int:8")));
      vars.insert (std::make_pair ("$request", new CVar ("$request", "GET|POST|PUT|PATCH|DELETE")));
      vars.insert (std::make_pair ("$request_completion", new CVar ("$request_completion", "ok|")));
      vars.insert (std::make_pair ("$request_id", new CVar ("$request", "@hex:16")));
      vars.insert (std::make_pair ("$request_time", new CVar ("$request_time", "0.001..60.000")));
      vars.insert (std::make_pair ("$server_protocol", new CVar ("$server_protocol", "HTTP/1.0|HTTP/1.1|HTTP/2.0")));
      vars.insert (std::make_pair ("$scheme", new CVar ("$scheme", "http|https")));
      vars.insert (std::make_pair ("$status", new CVar ("$status", "200|400")));
      vars.insert (std::make_pair ("$time_iso8601", new CVar ("$time_iso8601", "@datetime:%Y-%m-%dT%H:%M:%SZ"))); //don't redefine
      vars.insert (std::make_pair ("$time_local", new CVar ("$time_local", "@datetime:%d/%b/%Y:%H:%M:%S %z")));
      vars.insert (std::make_pair ("$uri", new CVar ("$uri", "/|/favico.ico|/doc")));
     }



     //logstrings["apache"] = "\"%h %l %u %t \"%r\" %>s %b\"";

  if (mode == "apache")
     {
      vars.insert (std::make_pair ("$logstring", new CVar ("$logstring", logstrings["apache"])));

      vars.insert (std::make_pair ("%h", new CVar ("%h", "@ip")));
      vars.insert (std::make_pair ("%l", new CVar ("%l", "@str:8|-")));
      vars.insert (std::make_pair ("%u", new CVar ("%u", "@str:8|-")));
      vars.insert (std::make_pair ("%t", new CVar ("%t", "@datetime:%d/%b/%Y:%H:%M:%S %z")));

      //$request $uri $protocol
      vars.insert (std::make_pair ("%r", new CVar ("%r", "@meta:(@seq:/GET:/PUT) (@path:1:5:3) (@seq:HTTP/1.1:/HTTP/2.0)")));

      vars.insert (std::make_pair ("%>s", new CVar ("%>s", "200|400")));
      vars.insert (std::make_pair ("%b", new CVar ("%b", "1..9999")));


      //      vars.insert (std::make_pair ("%r", new CVar ("%r", "/GET /hello.html HTTP/1.1.")));

     }




  for (map <string, string>::const_iterator it = pf->values.begin(); it != pf->values.end(); it++)
      {
       auto f = vars.find (it->first);
       if (f != vars.end()) //delete if variable pre-defined
          {
           delete f->second;
           vars.erase (f);
          }

      vars.insert (std::make_pair (it->first, new CVar (it->first, it->second)));
     }


}


string CTpl::prepare_log_string()
{

// std::cout << "111" << std::endl;


  string logstring = vars["$logstring"]->get_val();
  logstring.reserve (256);

//   std::cout << "222:" << logstring << std::endl;


  if (logstring.empty())
     {
      cout << "$logstring mandatory variable is not defined!" << endl;
      return "";
     }



//change to     for (auto it = mymap.rbegin(); it != mymap.rend(); it++)
  map <string, CVar*>::reverse_iterator it;
  for (it = vars.rbegin(); it != vars.rend(); it++)
      {
       string variable = it->first;

       size_t i = 0;
       do
         {
          i = logstring.find (variable);
          if (i != string::npos)
              logstring.replace (i, variable.length(), it->second->get_val());
         }
       while (i != string::npos);
      }



  return logstring;
}
