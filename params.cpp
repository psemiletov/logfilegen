#include <iostream>

#include "params.h"


void CParameters::print()
{

  cout << "------------ Print parameters -------------" << endl;

  cout << "benchmark: " << benchmark << endl;
  cout << "debug: " << debug << endl;
  cout << "duration: " << duration << endl;
  cout << "lines: " << lines << endl;
  cout << "logfile: " << logfile << endl;
  cout << "logsize: " << max_log_file_size << endl;
  cout << "max_log_files: " << max_log_files << endl;
  cout << "mode: " << mode << endl;
  cout << "pure: " << pure << endl;
  cout << "rate: " << rate << endl;
  cout << "size: " << size << endl;
  cout << "stdout: " << bstdout << endl;
  cout << "stats: " << stats << endl;
  cout << "template: " << templatefile << endl;

  cout << "------------ **************** -------------" << endl;
}
