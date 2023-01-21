/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/


#include <cstdint>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <stdio.h>  // for FILENAME_MAX
#include <sys/stat.h>
#include <csignal>


#include "pairfile.h"
#include "utl.h"
#include "tpl.h"
#include "cycle.h"
#include "params.h"


#ifndef VERSION_NUMBER
#define VERSION_NUMBER "0.4.1"
#endif


using namespace std;
using namespace std::chrono;


string find_config_in_paths (const string &fname)
{
  if (is_path_abs (fname))
     if (file_exists (fname))
        return fname;

  string fname_config = current_path() + "/" + fname;

  if (! file_exists (fname_config))
      fname_config = get_home_dir() + "/.config/logfilegen/" + fname;

  if (! file_exists (fname_config))
      fname_config =  "/etc/logfilegen/"+ fname;

  if (! file_exists (fname_config))
     return string();

  return fname_config;
}



void show_version()
{
  cout << "logfilegen v." << VERSION_NUMBER << endl;
}



void show_help()
{
  cout << "HELP!" << endl;
}


int main (int argc, char *argv[])
{
//  cout << "version: " << VERSION_NUMBER << endl;


  vector <string> envars = {"LFG_DURATION", "LFG_RATE", "LFG_LOGFILE",
                            "LFG_TEMPLATE", "LFG_DEBUG", "LFG_PURE",
                            "LFG_LOGSIZE", "LFG_LOGCOUNT", "LFG_GZIP", "LFG_LINES", "LFG_SIZE", "LFG_RANDOM"};

  CParameters params;
  string fname_config;


  for (int i = 0; i < argc; i++)
     {
      string ts = argv[i];
      size_t pos =  ts.find ("--config");
      if (pos != string::npos)
        {
         size_t eql = ts.find ("=");
         if (eql != string::npos)
            {
             string fname = ts.substr (eql + 1);
             fname_config = find_config_in_paths (fname);
            }
        }
     }


//Try to load params from config
/*

  string fname_config = "/etc/logfilegen/logfilegen.conf";

  if (! file_exists (fname_config))
      fname_config = get_home_dir() + "/.config/logfilegen/logfilegen.conf";

  if (! file_exists (fname_config))
      fname_config = current_path() + "/logfilegen.conf";
*/

    if (fname_config.empty())
       fname_config = find_config_in_paths ("logfilegen.conf");

//   cout << "Load parameters from config file: " << fname_config << endl;

   //load params from config:
   CPairFile opts_config (fname_config);

   params.lines = opts_config.get_uint ("lines", 0);
   //params.lines_unrated = opts_config.get_uint ("linesunrated", 0);
   params.s_size = opts_config.get_string ("size", "0");

   params.timestamp = opts_config.get_string ("timestamp", "%d/%b/%Y:%H:%M:%S %z");
   params.random = opts_config.get_bool ("random", false);


   params.duration = opts_config.get_int ("duration", 0);
   params.rate = opts_config.get_int ("rate", 0);
   params.logfile = opts_config.get_string ("logfile", "stdout");
   params.templatefile = opts_config.get_string ("template", "NOTEMPLATEFILE");
   params.mode = opts_config.get_string ("mode", "nginx");
   params.pure = opts_config.get_bool ("pure", false);
   params.debug = opts_config.get_bool ("debug", false);
   params.use_gzip = opts_config.get_bool ("gzip", false);

   params.max_log_files = opts_config.get_int ("logcount", 5);
   params.max_log_file_size = opts_config.get_string ("logsize", "16m");


   if (params.debug)
      params.print();

// Load params from command line

   if (params.debug)
      cout << "Load params from command line" << endl;


  CPairFile opts_cmdline (argc, argv);

//  params.lines_unrated = opts_cmdline.get_uint ("linesunrated", params.lines_unrated);

  params.timestamp = opts_cmdline.get_string ("timestamp", params.timestamp);

  params.s_size = opts_cmdline.get_string ("size", params.s_size);

  params.lines = opts_cmdline.get_uint ("lines", params.lines);
  params.duration = opts_cmdline.get_int ("duration", params.duration);
  params.rate = opts_cmdline.get_int ("rate", params.rate);
  params.logfile = opts_cmdline.get_string ("logfile", params.logfile);
  params.templatefile = opts_cmdline.get_string ("template", params.templatefile);
  params.mode = opts_cmdline.get_string ("mode", params.mode);
  params.pure = opts_cmdline.get_bool ("pure", params.pure);
  params.random = opts_cmdline.get_bool ("random", params.random);


  params.use_gzip = opts_cmdline.get_bool ("gzip", params.use_gzip);

  params.debug = opts_cmdline.get_bool ("debug", params.debug);

  params.max_log_files = opts_cmdline.get_int ("logcount", params.max_log_files);
  params.max_log_file_size = opts_cmdline.get_string ("logsize", params.max_log_file_size);


  if (opts_cmdline.get_bool ("help", false))
    {
     show_help();
     return 0;
    }

  if (opts_cmdline.get_bool ("version", false))
    {
     show_version();
     return 0;
    }


 if (params.debug)
    params.print();

// load params from ENV

  if (params.debug)
     cout << "Load ENV" << endl;


  CPairFile opts_envars (envars);

  params.s_size = opts_envars.get_string ("size", params.saffectsaffectsaffe_size);
  params.timestamp = opts_envars.get_string ("timestamp", params.timestamp);


  params.lines = opts_envars.get_uint ("lines", params.lines);
  params.duration = opts_envars.get_int ("duration", params.duration);
  params.rate = opts_envars.get_int ("rate", params.rate);
  params.logfile = opts_envars.get_string ("logfile", params.logfile);
  params.templatefile = opts_envars.get_string ("template", params.templatefile);
  params.mode = opts_envars.get_string ("mode", params.mode);
  params.pure = opts_envars.get_bool ("pure", params.pure);

  params.debug = opts_envars.get_bool ("debug", params.debug);
  params.use_gzip = opts_envars.get_bool ("gzip", params.use_gzip);
  params.random = opts_envars.get_bool ("random", params.random);


  params.max_log_files = opts_envars.get_int ("logcount", params.max_log_files);
  params.max_log_file_size = opts_envars.get_string ("logsize", params.max_log_file_size);

  params.size = string_to_file_size (params.s_size);


  if (params.debug)
      params.print();

  if (params.rate == 0 && params.size == 0 && params.duration == 0 && params.lines == 0)
     {
      cout << "Wrong parameters, please read the manual" << endl;
      return 0;
     }


  if (params.logfile == "stdout")
     params.bstdout = true;
  else
      params.bstdout = false;

  //if (params.logfile[0] != '/')          //path is local
   if (! is_path_abs (params.logfile))
      params.logfile = current_path() + "/" + params.logfile;


//read template

  string fname_template = params.templatefile;

  if (params.templatefile != "NOTEMPLATEFILE" && ! params.templatefile.empty())
     if (! is_path_abs (params.templatefile))

//     if (params.templatefile[0] != '/') //path is not absolute
        {
         fname_template = current_path() + "/" + params.templatefile;

         if (! file_exists (fname_template))
            fname_template = current_path() + "/templates/" + params.templatefile;

         if (! file_exists (fname_template))
            fname_template = get_home_dir() + "/.config/logfilegen/templates/" + params.templatefile;

         if (! file_exists (fname_template))
            fname_template = "/etc/logfilegen/templates/" + params.templatefile;

         if (! file_exists (fname_template))
            {
             cout << "No template file " << fname_template << " found, exiting" << endl;
             return 0;
            }

       }

  if (params.templatefile[0] == '/') //path is absolute
     if (! file_exists (fname_template))
        {
         cout << "No template file " << fname_template << " found, exiting" << endl;
         return 0;
        }



//  cout << "fname_template: " << fname_template << endl;
/*
     CGenCycleRated cycle (&params, fname_template);
     if (cycle.open_logfile())
        cycle.loop();
  */

/*
 if (params.lines_unrated == 0)
    {
     CGenCycleRated cycle (&params, fname_template);
     if (cycle.open_logfile())
        cycle.loop();
    }
 else
    {
     CGenCycleUnrated cycle (&params, fname_template);
     if (cycle.open_logfile())
        cycle.loop();
    }
*/




 if (params.rate == 0)
    {
     CGenCycleUnrated cycle (&params, fname_template);
     if (cycle.open_logfile())
        cycle.loop();
    }
 else
    {
     CGenCycleRated cycle (&params, fname_template);
     if (cycle.open_logfile())
        cycle.loop();
    }


  return 0;
}

