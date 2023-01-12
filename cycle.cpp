#include "cycle.h"



namespace
{
  volatile std::sig_atomic_t g_signal;
}


void f_signal_handler (int signal)
{
  g_signal = signal;
  cout << "SIGNAL!!!" << endl;
}



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



CGenCycleUnrated::CGenCycleUnrated (CParameters *prms, const string &fname)
{
  params = prms;
  fname_template = fname;

}



CGenCycleUnrated::~CGenCycleUnrated()
{
  delete tpl;

}


bool CGenCycleUnrated::init()
{
  std::signal (SIGINT, f_signal_handler);


  tpl = new CTpl (fname_template, params->mode);

  file_out_error = false;

  if (! params->bstdout)
    {
     file_out.open (params->logfile, std::ios::out | std::ios::app);

     if (file_out.fail())
        {
      //  throw std::ios_base::failure(std::strerror(errno));

        file_out_error = true;
        cout << "cannot create " << params->logfile << "\n";
       }
    else
        file_out.exceptions (file_out.exceptions() | std::ios::failbit | std::ifstream::badbit);
    }



  if (! params->bstdout)
     {

     //  how many space we occupy with logstrings?

     size_t free_space = get_free_space (get_file_path (params->logfile));

     string test_string = tpl->prepare_log_string();
     size_t test_string_size = test_string.size() + (test_string.size() / 2);

     //  cout << "test_string_size, bytes: " << test_string_size  << endl;

     std::uintmax_t lines_total = static_cast<std::uintmax_t> (params->duration) * params->rate;

     if (params->debug)
        cout << "lines_total: " << lines_total  << endl;


     std::uintmax_t size_needed = lines_total * test_string_size;

     if (params->debug)
        cout << "size_needed, bytes: " << size_needed << endl;


     if (size_needed >= free_space/*sinfo.available*/ )
       {
        //exit from program

        cout << "Output file will be too large with current parameters, exiting!" << endl;
        return false;
       }
    }


   return true;

}



void CGenCycleUnrated::loop()
{
   auto start = high_resolution_clock::now();

   int seconds_counter = 0;
   int frame_counter = 0;

   using clock = std::chrono::steady_clock;

   auto next_frame = clock::now();

   while (true)
         {
          next_frame += std::chrono::nanoseconds (1000000000 / params->rate);

          frame_counter++;

//          std::cout << "seconds_counter: " << seconds_counter << endl;
  //        std::cout << "frame_counter: " << frame_counter << endl;


          if (frame_counter == params->rate)
             {
              frame_counter = 0;
              seconds_counter++;
             }

          if (seconds_counter == params->duration)
             break;

           if (g_signal == SIGINT)
             {
          //    cout << "break the main loop" << endl;
              break;
             }

          string log_string = tpl->prepare_log_string();

          if (! params->pure)
             {
              if (params->bstdout)
                 cout << log_string << "\n";

              if (! file_out_error)
                 file_out << log_string << "\n";
              //  pw.write (log_string);

             }
         // std::cout << std::time(0) << endl;

          std::this_thread::sleep_until (next_frame);
         }


  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  auto duration_s = duration_cast<seconds>(stop - start);

  if (params->debug)
     {
      cout << "duration.count (microseconds): " << duration.count() << endl;
      cout << "duration_s.count (seconds): " << duration_s.count() << endl;
     }



}
