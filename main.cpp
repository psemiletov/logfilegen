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


#include <sys/statvfs.h>

#ifdef WINDOWS
#include <direct.h>
#define get_cur_dir _getcwd
#else
#include <unistd.h>
#define get_cur_dir getcwd
#endif


#include "libretta_pairfile.h"
#include "tpl.h"


#ifndef VERSION_NUMBER
#define VERSION_NUMBER "0.0.x"
#endif


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

  bool pure;
  bool bstdout;
  bool debug;

  void print();
};


void CParameters::print()
{

  cout << "------------ Print parameters -------------" << endl;


  cout << "duration: " << duration << endl;
  cout << "rate: " << rate << endl;
  cout << "templatefile: " << templatefile << endl;
  cout << "logfile: " << logfile << endl;


  cout << "pure: " << pure << endl;

  cout << "mode: " << mode << endl;
  cout << "debug: " << debug << endl;
  cout << "stdout: " << bstdout << endl;

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


size_t get_free_space (const string &path)
{
  cout << "get_free_space for " << path << endl;

  struct statvfs buf;

  int r = statvfs (path.c_str(), &buf);

  if (r < 0)
     return -1;

  return buf.f_bavail * buf.f_bsize;
}


string get_file_path (const string &path)
{
  char sep = '/';

#ifdef _WIN32
   sep = '\\';
#endif

  size_t i = path.rfind (sep, path.length());

  if (i != string::npos)
     return path.substr(0, i);

  return("");
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
//  cout << "version: " << VERSION_NUMBER << endl;

  vector <string> envars = {"LFG_DURATION", "LFG_RATE", "LFG_LOGFILE", "LFG_TEMPLATEFILE", "LFG_DEBUG", "LFG_PURE"};

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


//   cout << "Load parameters from config file: " << fname_config << endl;

   //load params from config:
   CPairFile opts_config (fname_config);

   params.duration = opts_config.get_int ("duration", 2);
   params.rate = opts_config.get_int ("rate", 3);
   params.logfile = opts_config.get_string ("logfile", "stdout");
   params.templatefile = opts_config.get_string ("templatefile", "test.tp");
   params.mode = opts_config.get_string ("mode", "nginx");
   params.pure = opts_config.get_bool ("pure", false);
   params.debug = opts_config.get_bool ("debug", false);


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


  //params.print();

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

  //params.print();


  if (params.logfile == "stdout")
     params.bstdout = true;
  else
      params.bstdout = false;

  if (params.logfile[0] != '/')
         //path is local
      params.logfile = current_path() + "/" + params.logfile;


//read template

   string fname_template = params.templatefile;

  if (! params.templatefile.empty())
     if (params.templatefile[0] != '/') //path is not absolute
       {
        fname_template = "/etc/logfilegen/templates/" + params.templatefile;

        if (! file_exists (fname_template))
           fname_template = get_home_dir() + "/.config/logfilegen/templates/" + params.templatefile;

        if (! file_exists (fname_template))
          fname_template = current_path() + "/templates/" + params.templatefile;
       }

  //cout << "fname_template: " << fname_template << endl;

  CTpl tpl (fname_template, params.mode);


  ofstream file_out;
  bool file_out_error = false;



  if (! params.bstdout)
    {
     file_out.open (params.logfile, std::ios::out | std::ios::app);

     if (file_out.fail())
        {
      //  throw std::ios_base::failure(std::strerror(errno));

        file_out_error = true;
        cout << "cannot create " << params.logfile << "\n";
       }
    else
        file_out.exceptions (file_out.exceptions() | std::ios::failbit | std::ifstream::badbit);
    }


//check is there free space on disk where logfile will be created

 //params.logfile

//get_file_path

  //cout << "get_file_path(params.logfile): " << get_file_path(params.logfile) << endl;

  //cout << "...................... get_free_space: " << get_free_space (get_file_path(params.logfile)) << endl;


 //C++ 17

  if (! params.bstdout)
     {
     //std::filesystem::__cxx11::path logpath = current_path();
     //filesystem::space_info sinfo = std::filesystem::space (logpath);
     //cout << "Free space on " << logpath << ": " << sinfo.available << " bytes" << endl;

     //  how many space we occupy with logstrings?

     size_t free_space = get_free_space (get_file_path(params.logfile));

     string test_string = tpl.prepare_log_string();
     size_t test_string_size = test_string.size() + (test_string.size() / 2);

     //  cout << "test_string_size, bytes: " << test_string_size  << endl;


     std::uintmax_t lines_total = params.duration * params.rate;

     if (params.debug)
        cout << "lines_total: " << lines_total  << endl;


     std::uintmax_t size_needed = lines_total * test_string_size;

     if (params.debug)
        cout << "size_needed, bytes: " << size_needed << endl;


     if (size_needed >= free_space/*sinfo.available*/ )
       {
        //exit from program

        cout << "Output file will be too large with current parameters, exiting!" << endl;
        return 0;
       }
    }

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
          next_frame += std::chrono::nanoseconds (1000000000 / params.rate);

          frame_counter++;

//          std::cout << "seconds_counter: " << seconds_counter << endl;
  //        std::cout << "frame_counter: " << frame_counter << endl;


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

          string log_string = tpl.prepare_log_string();

          if (! params.pure)
             {
              if (params.bstdout)
                 cout << log_string << "\n";

              if (! file_out_error)
                 file_out << log_string << "\n";

          }
         // std::cout << std::time(0) << endl;

          std::this_thread::sleep_until (next_frame);
         }


  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  auto duration_s = duration_cast<seconds>(stop - start);

  if (params.debug)
     {
      cout << "duration.count (microseconds): " << duration.count() << endl;
      cout << "duration_s.count (seconds): " << duration_s.count() << endl;
     }

  return 0;
}
