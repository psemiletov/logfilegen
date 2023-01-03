/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/


#include <cstdint>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <stdio.h>  // for FILENAME_MAX
#include <sys/stat.h>
#include <csignal>

#ifdef WINDOWS
#include <direct.h>
#define get_cur_dir _getcwd
#else
#include <unistd.h>
#define get_cur_dir getcwd
#endif


#include "libretta_pairfile.h"
#include "tpl.h"


using namespace std;
using namespace std::chrono;



#ifdef WINDOWS
#define DIR_SEPARATOR '\\'
#else
#define DIR_SEPARATOR '/'
#endif


class CParameters
{
public:

  string templatefile; //templatefile name from /logfilegen/templates
  string logfile; //output logfile name with full path
  string mode;    //"nginx" or "apache"

  int duration; //duration of log generation, in seconds
  int rate;  //during the log generation, how many lines per second will be written

  bool append;
  bool create;
  bool verbose;

  void print();
};


void CParameters::print()
{
  cout << "------------ Print parameters -------------" << endl;

  cout << "duration: " << duration << endl;
  cout << "rate: " << rate << endl;
  cout << "templatefile: " << templatefile << endl;
  cout << "logfile: " << logfile << endl;


  cout << "mode: " << mode << endl;
  cout << "append: " << append << endl;
  cout << "create: " << create << endl;
  cout << "verbose: " << verbose << endl;

  cout << "------------ **************** -------------" << endl;

}


namespace
{
  volatile std::sig_atomic_t g_signal_status;
}


void signal_handler (int signal)
{
  g_signal_status = signal;
}


string get_home_dir()
{
  string homedir = getenv ("HOME");
  return homedir;
}


string current_path()
{
  char path [FILENAME_MAX];
  string result;

  if (! get_cur_dir (path, sizeof (path)))
     return result;

  result = path;
  return result;
}


inline bool file_exists (const std::string& name)
{
  struct stat buffer;
   return (stat (name.c_str(), &buffer) == 0);
}


int main (int argc, char *argv[])
{

  vector <string> envars = {"LFG_DURATION", "LFG_RATE", "LFG_LOGFILE", "LFG_TEMPLATEFILE"};

  CParameters params;

  std::signal (SIGINT, signal_handler);


/*
Params initialization order and overrides:

1. config file
2. command line parameters
3. ENV variables

*/

//Try to load params from config

  string fname_config = "/etc/logfilegen/logfilegen.conf";

  if (! file_exists (fname_config))
      fname_config = get_home_dir() + "/.config/logfilegen/logfilegen.conf";

  if (! file_exists (fname_config))
      fname_config = current_path() + "/logfilegen.conf";

  if (file_exists (fname_config))
     {
      cout << "Load parameters from config file: " << fname_config << endl;

      //load params from config:
      CPairFile opts_config (fname_config);

      params.duration = opts_config.get_int ("duration", 3);
      params.rate = opts_config.get_int ("rate", 5);
      params.logfile = opts_config.get_string ("logfile", "test.log");
      params.templatefile = opts_config.get_string ("templatefile", "test.tp");
      params.mode = opts_config.get_string ("mode", "nginx");

     }
   else
       cout << "No config file exists" << endl;

// Load params from command line

  CPairFile opts_cmdline (argc, argv);

  params.duration = opts_cmdline.get_int ("duration", params.duration);
  params.rate = opts_cmdline.get_int ("rate", params.rate);
  params.logfile = opts_cmdline.get_string ("logfile", params.logfile);
  params.templatefile = opts_cmdline.get_string ("templatefile", params.templatefile);
  params.mode = opts_cmdline.get_string ("mode", params.mode);


// load params from ENV

  CPairFile opts_envars (envars);

  params.duration = opts_envars.get_int ("duration", params.duration);
  params.rate = opts_envars.get_int ("rate", params.rate);
  params.logfile = opts_envars.get_string ("logfile", params.logfile);
  params.templatefile = opts_envars.get_string ("templatefile", params.templatefile);
  params.mode = opts_envars.get_string ("mode", params.mode);


  params.print();


//read template

  string fname_template;
  fname_template = "/etc/logfilegen/templates/" + params.templatefile;

  if (! file_exists (fname_template))
      fname_template = get_home_dir() + "/.config/logfilegen/templates/" + params.templatefile;

  if (! file_exists (fname_template))
      fname_template = current_path() + "/templates/" + params.templatefile;


  cout << "fname_template: " << fname_template << endl;



  CTpl tpl (fname_template, params.mode);


/*******************************
MAIN LOOP
******************************/


   auto start = high_resolution_clock::now();

   int seconds_counter = 0;
   int frame_counter = 0;

   using clock = std::chrono::steady_clock;

   auto next_frame = clock::now();

   while (true)
         {
          next_frame += std::chrono::milliseconds(1000 / params.rate);

          frame_counter++;

          //std::cout << "seconds_counter: " << seconds_counter << endl;
//          std::cout << "frame_counter: " << frame_counter << endl;

           //simple output to screen
          //cout << tpl.prepare_log_string() << endl;


          if (frame_counter == params.rate)
             {
              frame_counter = 0;
              seconds_counter++;
             }

          if (seconds_counter == params.duration)
             break;

           if (g_signal_status == SIGINT)
             {
              cout << "break the main loop" << endl;
              break;
             }


          //WRITE TO LOG HERE

          //simple output to screen
          //cout << tpl.prepare_log_string() << endl;

          cout << tpl.prepare_log_string() << "\n";


         // std::cout << std::time(0) << endl;

          std::this_thread::sleep_until(next_frame);
         }


  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  auto duration_s = duration_cast<seconds>(stop - start);

  cout << "duration.count (microseconds): " << duration.count() << endl;
  cout << "duration_s.count (seconds): " << duration_s.count() << endl;

  return 0;
}
