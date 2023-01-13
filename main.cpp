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


#ifndef VERSION_NUMBER
#define VERSION_NUMBER "0.2.0"
#endif


using namespace std;
using namespace std::chrono;




int main (int argc, char *argv[])
{
//  cout << "version: " << VERSION_NUMBER << endl;


  vector <string> envars = {"LFG_DURATION", "LFG_RATE", "LFG_LOGFILE",
                            "LFG_TEMPLATEFILE", "LFG_DEBUG", "LFG_PURE",
                            "LFG_MAX_LOG_FILE_SIZE", "LFG_MAX_LOG_FILES"};

  CParameters params;


//Try to load params from config


  string fname_config = "/etc/logfilegen/logfilegen.conf";

  if (! file_exists (fname_config))
      fname_config = get_home_dir() + "/.config/logfilegen/logfilegen.conf";

  if (! file_exists (fname_config))
      fname_config = current_path() + "/logfilegen.conf";


//   cout << "Load parameters from config file: " << fname_config << endl;

   //load params from config:
   CPairFile opts_config (fname_config);

   params.duration = opts_config.get_int ("duration", 2);
   params.rate = opts_config.get_int ("rate", 3);
   params.logfile = opts_config.get_string ("logfile", "stdout");
   params.templatefile = opts_config.get_string ("templatefile", "NOTEMPLATEFILE");
   params.mode = opts_config.get_string ("mode", "nginx");
   params.pure = opts_config.get_bool ("pure", false);
   params.debug = opts_config.get_bool ("debug", false);
   params.max_log_files = opts_config.get_int ("max_log_files", 5);
   params.max_log_file_size = opts_config.get_string ("max_log_file_size", "64k");


   if (params.debug)
      params.print();

// Load params from command line

   if (params.debug)
      cout << "Load params from command line" << endl;


  CPairFile opts_cmdline (argc, argv);

  params.duration = opts_cmdline.get_int ("duration", params.duration);
  params.rate = opts_cmdline.get_int ("rate", params.rate);
  params.logfile = opts_cmdline.get_string ("logfile", params.logfile);
  params.templatefile = opts_cmdline.get_string ("templatefile", params.templatefile);
  params.mode = opts_cmdline.get_string ("mode", params.mode);
  params.pure = opts_cmdline.get_bool ("pure", params.pure);

  params.max_log_files = opts_cmdline.get_int ("max_log_files", params.max_log_files);
  params.max_log_file_size = opts_cmdline.get_string ("max_log_file_size", params.max_log_file_size);




 if (params.debug)
    params.print();

// load params from ENV

  if (params.debug)
     cout << "Load ENV" << endl;


  CPairFile opts_envars (envars);

  params.duration = opts_envars.get_int ("duration", params.duration);
  params.rate = opts_envars.get_int ("rate", params.rate);
  params.logfile = opts_envars.get_string ("logfile", params.logfile);
  params.templatefile = opts_envars.get_string ("templatefile", params.templatefile);
  params.mode = opts_envars.get_string ("mode", params.mode);
  params.pure = opts_envars.get_bool ("pure", params.pure);


  params.max_log_files = opts_envars.get_int ("max_log_files", params.max_log_files);
  params.max_log_file_size = opts_envars.get_string ("max_log_file_size", params.max_log_file_size);

  if (params.debug)
      params.print();


  if (params.logfile == "stdout")
     params.bstdout = true;
  else
      params.bstdout = false;

  if (params.logfile[0] != '/')          //path is local
      params.logfile = current_path() + "/" + params.logfile;


//read template

  string fname_template = params.templatefile;

  if (params.templatefile != "NOTEMPLATEFILE" && ! params.templatefile.empty())
     if (params.templatefile[0] != '/') //path is not absolute
        {
         fname_template = "/etc/logfilegen/templates/" + params.templatefile;

         if (! file_exists (fname_template))
            fname_template = get_home_dir() + "/.config/logfilegen/templates/" + params.templatefile;

         if (! file_exists (fname_template))
            fname_template = current_path() + "/templates/" + params.templatefile;

         if (! file_exists (fname_template))
            fname_template = current_path() + "/" + params.templatefile;

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

  CGenCycleUnrated cycle (&params, fname_template);
  if (cycle.open_logfile())
     cycle.loop();

  return 0;
}

