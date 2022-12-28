#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>


#include "libretta_pairfile.h"


int main (int argc, char *argv[])
{
  using namespace std;
  using namespace std::chrono;


  size_t seconds_to_generate;
  size_t lines_per_second;

  CPairFile opts_cmdline (argc, argv);

  seconds_to_generate = opts_cmdline.get_int ("--seconds_to_generate", 3);

  lines_per_second = opts_cmdline.get_int ("--lines_per_second", 5);

 // cout << opts_cmdline.get_string ("--seconds_to_generate");

   size_t seconds_counter = 0;
   size_t frame_counter = 0;

   using clock = std::chrono::steady_clock;



    auto next_frame = clock::now();

    while (true)
    {
//        next_frame += std::chrono::milliseconds(1000 / 5); // 5Hz
        next_frame += std::chrono::milliseconds(1000 / lines_per_second); // 5Hz

        std::cout << "seconds_counter: " << seconds_counter << endl; // 5 for each second

        std::cout << "frame_counter: " << frame_counter++ << endl; // 5 for each second

        if (frame_counter == lines_per_second)
           {
            frame_counter = 0;
            seconds_counter++;
           }

        if (seconds_counter == seconds_to_generate)
           break;

     //   cout << "frame: " << next_frame << endl; // 5 for each second


        // do stuff
        std::cout << std::time(0) << '\n'; // 5 for each second

        // wait for end of frame
        std::this_thread::sleep_until(next_frame);
    }


  return 0;
}
