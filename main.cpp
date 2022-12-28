#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <stdio.h>  // for FILENAME_MAX
#include <sys/stat.h>

//#include <dirent.h>

#ifdef WINDOWS
#include <direct.h>
#define get_cur_dir _getcwd
#else
#include <unistd.h>
#define get_cur_dir getcwd
#endif


#include "libretta_pairfile.h"

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

  size_t seconds_to_generate;
  size_t lines_per_second;
  string logfile; //logfile name with full path
  string mode;    //nginx or apache


  bool append;
  bool create;
};


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


  CPairFile opts_cmdline (argc, argv);


  CParameters params;

/*
Params initialization order and overrides:

1. config file
2. command line parameters
3. ENV variables

*/

//Try to load params from config

  string fname_config = current_path() + "/logfilegen.conf";

  if (! file_exists (fname_config))
     fname_config = "/etc/logfilegen.conf";

    if (! file_exists (fname_config))
      fname_config = get_home_dir() + "/.config/logfilegen.conf";

  if (file_exists (fname_config))
     {
      //load params from config:
      CPairFile opts_config (fname_config);

      params.seconds_to_generate = opts_config.get_int ("--seconds_to_generate", 3);
      params.lines_per_second = opts_config.get_int ("--lines_per_second", 5);

     }



// Load params from command line
  params.seconds_to_generate = opts_cmdline.get_int ("--seconds_to_generate", 3);
  params.lines_per_second = opts_cmdline.get_int ("--lines_per_second", 5);

 // cout << opts_cmdline.get_string ("--seconds_to_generate");

   size_t seconds_counter = 0;
   size_t frame_counter = 0;

   using clock = std::chrono::steady_clock;

   auto next_frame = clock::now();

    while (true)
    {
        next_frame += std::chrono::milliseconds(1000 / params.lines_per_second);

        std::cout << "seconds_counter: " << seconds_counter << endl;
        std::cout << "frame_counter: " << frame_counter++ << endl;

        if (frame_counter == params.lines_per_second)
           {
            frame_counter = 0;
            seconds_counter++;
           }

        if (seconds_counter == params.seconds_to_generate)
           break;



        //WRITE TO LOG HERE


        std::cout << std::time(0) << endl; // 5 for each second

        // wait for end of frame
        std::this_thread::sleep_until(next_frame);
    }


  return 0;
}
