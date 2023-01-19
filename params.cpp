#include "params.h"

#include <iostream>




void CParameters::print()
{

  cout << "------------ Print parameters -------------" << endl;

  cout << "lines: " << lines << endl;

  cout << "duration: " << duration << endl;
  cout << "rate: " << rate << endl;
  cout << "templatefile: " << templatefile << endl;
  cout << "logfile: " << logfile << endl;

  cout << "max_log_files: " << max_log_files << endl;
  cout << "logsize: " << max_log_file_size << endl;
  cout << "size: " << size << endl;


  cout << "pure: " << pure << endl;
  cout << "mode: " << mode << endl;
  cout << "debug: " << debug << endl;
  cout << "stdout: " << bstdout << endl;

  cout << "------------ **************** -------------" << endl;
}

