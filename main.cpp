/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

//#undef _HAS_STD_BYTE

#include <thread>

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <iomanip>

//#include <filesystem>
/*
#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __cplusplus >= 201703L && __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  endif
#endif
*/


#include "pairfile.h"
#include "utl.h"
#include "tpl.h"
#include "cycle.h"
#include "params.h"


#ifndef VERSION_NUMBER
#define VERSION_NUMBER "2.3.0"
#endif



#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"


//using namespace std;

std::string find_config_in_paths (const std::string &fname)
{
  if (is_path_abs (fname))
     if (file_exists (fname))
        return fname;

  std::string fname_config = current_path() + "/" + fname;

  if (! file_exists (fname_config))
      fname_config = get_home_dir() + "/.config/logfilegen/" + fname;

  if (! file_exists (fname_config))
      fname_config =  "/etc/logfilegen/"+ fname;

  if (! file_exists (fname_config))
     return std::string();

  return fname_config;
}



void show_version()
{
  std::cout << "logfilegen v." << VERSION_NUMBER << std::endl;
}


void show_help()
{
  printf (GRN "logfilegen v%s by Peter Semiletov\n" RESET, VERSION_NUMBER);
  printf (YEL "Please read the full Manual at https://psemiletov.github.io/logfilegen/\n" RESET);
  printf (MAG "Parameters short list:\n");
  printf (CYN"--pure" RESET "      - Generate lines in memory or output them to the log file, disabled by default.\n");
  printf (CYN"--benchmark" RESET " - Perfrom benchmark to test logs generation performance of the current system, disabled by default.\n");
  printf (CYN"--duration" RESET "  - Logs generation duration in seconds, default is 0 and it means unlimited.\n");
  printf (CYN"--rate" RESET "      - Generation speed, default is 0 and it means unlimited.\n");
  printf (CYN"--lines" RESET "     - Generate number of lines and exit, overrides duration.\n");
  printf (CYN"--size" RESET "      - Generate specific log file size and exit, overrides duration and lines. Default units are bytes and we can specify k/m/g etc.\n");
  printf (CYN"--template" RESET "  - Template file name that is used for log file generation. See documentation for more details about template file format.\n");
  printf (CYN"--logfile" RESET "   - File name for the resulting logfile. We can also use 'stdout' to output lines to the console.\n");
  printf (CYN"--logsize" RESET "   - Maximum log file size, if exceeded, the log file rotation will happen. Default is 16m, default units are bytes and we can specify k/m/g\n");
  printf ("etc.\n");
  printf (CYN"--logcount" RESET "  - Number of the files to retain during rotation.\n");
  printf (CYN"--gzip" RESET "      - Use external gzip to compress rotated log files, disabled by default.\n");
  printf (CYN"--debug" RESET "     - Enable or disable debug, disabled by default.\n");
  printf ("\n");
  printf (MAG "Examples:\n" RESET);
  printf ("\n");
  printf ("# Run for 60 seconds with a rate 1000 lines per second and use pre-define Nginx template:\n");
  printf ("\n");
  printf ("logfilegen --duration=60 --rate=1000 --mode=nginx\n");
  printf ("\n");
  printf ("# Run for 60 seconds with a rate 1000 lines per second and use a custom Nginx template:\n");
  printf ("\n");
  printf ("logfilegen --duration=60 --rate=1000 --template=nginx.tpl --logfile=access.log\n");
}


void do_task (CGenCycle *c)
{
  c->loop();
}


int main (int argc, char *argv[])
{
//  cout << "version: " << VERSION_NUMBER << endl;

//  std::string tdir = fs::temp_directory_path().string();

   std::string tdir = get_tmp_dir();


  std::string temp_logfile;
  std::string temp_logfile0;

  if (! tdir.empty())
     {
      temp_logfile = tdir + "/" + "logfilegen.log";
      temp_logfile0 = tdir + "/" + "logfilegen.log.0";
      remove (temp_logfile.c_str());
      remove (temp_logfile0.c_str());
     }


  std::vector <std::string> envars = {"LFG_DURATION", "LFG_RATE", "LFG_LOGFILE",
                            "LFG_TEMPLATE", "LFG_DEBUG", "LFG_PURE",
                            "LFG_LOGSIZE", "LFG_LOGCOUNT", "LFG_GZIP",
                            "LFG_LINES", "LFG_SIZE", "LFG_RANDOM",
                            "LFG_BENCHMARK", "LFG_STATS", "LFG_TEST",
                            "LFG_ADDR", "LFG_METRICS", "LFG_PORT",
                            "LFG_IP", "LFG_RESULTS", "LFG_RESULTS_TEMPLATE"};

  CParameters params;
  std::string fname_config;


  for (int i = 0; i < argc; i++)
      {
       std::string ts = argv[i];
       size_t pos =  ts.find ("--config");
       if (pos != std::string::npos)
          {
           size_t eql = ts.find ("=");
           if (eql != std::string::npos)
              {
               std::string fname = ts.substr (eql + 1);
               fname_config = find_config_in_paths (fname);
              }
          }
      }


//Try to load params from config

    if (fname_config.empty())
        fname_config = find_config_in_paths ("logfilegen.conf");

//   cout << "Load parameters from config file: " << fname_config << endl;

   //load params from config:
   CPairFile opts_config (fname_config);
   params.metrics = opts_config.get_bool ("metrics", true);

   params.benchmark = opts_config.get_bool ("benchmark", false);
   params.debug = opts_config.get_bool ("debug", false);
   params.duration = opts_config.get_num ("duration", 0);
//   params.metrics_http_upd_poll = opts_config.get_num ("metrics_http_upd_poll", 5);



   params.poll = opts_config.get_num ("poll", 5);

   params.lines = opts_config.get_num ("lines", 0);
   params.port = opts_config.get_string ("port", "8080");
   params.ip = opts_config.get_string ("ip", "000.000.000.000");

   params.results = opts_config.get_string ("results", "");
   params.results_template = opts_config.get_string ("results_template", "@date - @duration - @mode/@template - @size_generated - @lines_generated - @performance_lps - @performance_bps");

   params.logfile = opts_config.get_string ("logfile", "stdout");
   params.max_log_file_size = opts_config.get_string ("logsize", "16m");
   params.max_log_files = opts_config.get_num ("logcount", 5);
   params.mode = opts_config.get_string ("mode", "nginx");
   params.pure = opts_config.get_bool ("pure", false);
   params.random = opts_config.get_bool ("random", false);
   params.rate = opts_config.get_int ("rate", 0);
   params.s_size = opts_config.get_string ("size", "0");
 //  params.stats = opts_config.get_bool ("stats", false);
   params.templatefile = opts_config.get_string ("template", "NOTEMPLATEFILE");

   params.metrics_addr = opts_config.get_string ("addr", "127.0.0.1:8080");



   params.timestamp = opts_config.get_string ("timestamp", "%d/%b/%Y:%H:%M:%S %z");
   params.use_gzip = opts_config.get_bool ("gzip", false);
   params.test = opts_config.get_bool ("test", false);

   if (params.debug)
      params.print();

// Load params from command line

   if (params.debug)
      std::cout << "Load params from command line" << std::endl;


  CPairFile opts_cmdline (argc, argv);

  params.benchmark = opts_cmdline.get_bool ("benchmark", params.benchmark);
  params.test = opts_cmdline.get_bool ("test", params.test);
  params.metrics = opts_cmdline.get_bool ("metrics", params.metrics);

  params.debug = opts_cmdline.get_bool ("debug", params.debug);
  params.duration = opts_cmdline.get_num ("duration", params.duration);
  params.poll = opts_cmdline.get_num ("poll", params.poll);

  //params.metrics_http_upd_poll = opts_cmdline.get_num ("metrics_http_upd_poll", params.metrics_http_upd_poll);



  params.lines = opts_cmdline.get_num ("lines", params.lines);
  params.logfile = opts_cmdline.get_string ("logfile", params.logfile);
  params.port = opts_cmdline.get_string ("port", params.port);
  params.ip = opts_cmdline.get_string ("ip", params.ip);

  params.results = opts_cmdline.get_string ("results", params.results);
  params.results_template = opts_cmdline.get_string ("results_template", params.results_template);


  params.max_log_file_size = opts_cmdline.get_string ("logsize", params.max_log_file_size);
  params.max_log_files = opts_cmdline.get_num ("logcount", params.max_log_files);
  params.mode = opts_cmdline.get_string ("mode", params.mode);
  params.pure = opts_cmdline.get_bool ("pure", params.pure);
  params.random = opts_cmdline.get_bool ("random", params.random);
  params.rate = opts_cmdline.get_num ("rate", params.rate);
  params.s_size = opts_cmdline.get_string ("size", params.s_size);
  params.metrics_addr = opts_cmdline.get_string ("addr", params.metrics_addr);

 // params.stats = opts_cmdline.get_bool ("stats", params.stats);
  params.templatefile = opts_cmdline.get_string ("template", params.templatefile);
  params.timestamp = opts_cmdline.get_string ("timestamp", params.timestamp);
  params.use_gzip = opts_cmdline.get_bool ("gzip", params.use_gzip);

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
     std::cout << "Load ENV" << std::endl;


  CPairFile opts_envars (envars);
  params.metrics = opts_envars.get_bool ("metrics", params.metrics);

  params.test = opts_envars.get_bool ("test", params.test);
  params.benchmark = opts_envars.get_bool ("benchmark", params.benchmark);
  params.debug = opts_envars.get_bool ("debug", params.debug);
  params.duration = opts_envars.get_num ("duration", params.duration);
  params.poll = opts_envars.get_num ("poll", params.poll);

  params.results = opts_envars.get_string ("results", params.results);
  params.results_template = opts_envars.get_string ("results_template", params.results_template);


  params.lines = opts_envars.get_num ("lines", params.lines);
  params.logfile = opts_envars.get_string ("logfile", params.logfile);
  params.port = opts_envars.get_string ("port", params.port);
  params.ip = opts_envars.get_string ("ip", params.ip);

  //params.metrics_http_upd_poll = opts_envars.get_num ("metrics_http_upd_poll", params.metrics_http_upd_poll);


  params.max_log_file_size = opts_envars.get_string ("logsize", params.max_log_file_size);
  params.max_log_files = opts_envars.get_num ("logcount", params.max_log_files);
  params.mode = opts_envars.get_string ("mode", params.mode);
  params.pure = opts_envars.get_bool ("pure", params.pure);
  params.random = opts_envars.get_bool ("random", params.random);
  params.rate = opts_envars.get_num ("rate", params.rate);
  params.s_size = opts_envars.get_string ("size", params.s_size);
  params.size = string_to_file_size (params.s_size);
//  params.stats = opts_envars.get_bool ("stats", params.stats);
  params.templatefile = opts_envars.get_string ("template", params.templatefile);
  params.timestamp = opts_envars.get_string ("timestamp", params.timestamp);
  params.use_gzip = opts_envars.get_bool ("gzip", params.use_gzip);
  params.metrics_addr = opts_envars.get_string ("addr", params.metrics_addr);

  if (params.max_log_files < 1)
     params.max_log_files = 1;


  if (params.debug)
      params.print();


  if (params.logfile == "stdout")
     params.bstdout = true;
  else
      params.bstdout = false;

   if (! is_path_abs (params.logfile))
       params.logfile = current_path() + "/" + params.logfile;


//read template

  std::string fname_template = params.templatefile;

  if (params.templatefile != "NOTEMPLATEFILE" && ! params.templatefile.empty())
     if (! is_path_abs (params.templatefile))
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
             std::cout << "No template file " << fname_template << " found, exiting" << std::endl;
             return 0;
            }
        }

  if (is_path_abs (params.templatefile))
     if (! file_exists (fname_template))
        {
         std::cout << "No template file " << fname_template << " found, exiting" << std::endl;
         return 0;
        }

  if (params.test)
     {
      params.benchmark = false;
      params.rate = 0;
      params.duration = 5;
      params.size = 0;
      params.lines = 0;
      params.max_log_files = 1;
      params.bstdout = false;

      //string tdir (std::filesystem::temp_directory_path());
      //params.logfile = tdir + "/" + "logfilegen.log";
      //cout << "params.logfile " << params.logfile << endl;

      params.logfile = temp_logfile;

      fname_template = "";
     }

  if (params.benchmark)
     {
      params.rate = 0;
      params.duration = 5;
      params.size = 0;
      params.lines = 0;
     }


   std::string fpath = get_file_path (params.logfile);
   if (! params.bstdout && ! file_exists(fpath))
      {
       std::cout << "error! " << fpath << " is not exists!" << std::endl;
       return 0;
      }


  if (params.rate == 0)
     {
      std::thread t1;
      std::thread t2;

      CGenCycleUnrated *cycle1 = new CGenCycleUnrated (&params, fname_template);
      if (cycle1->open_logfile())
          t1 = std::thread (do_task, cycle1);

      CGenCycleUnrated *cycle2 = new CGenCycleUnrated (&params, fname_template);
      if (cycle2->open_logfile())
          t2 = std::thread (do_task, cycle2);


      t1.join();
      delete cycle1;
      t2.join();
      delete cycle2;


     }
  else
      {
      CGenCycleRated cycle (&params, fname_template);
      if (cycle.open_logfile())
         cycle.loop();
     }

  if (params.test && ! tdir.empty())
     {
      remove (temp_logfile.c_str());
      remove (temp_logfile0.c_str());

     }

  return 0;
}
