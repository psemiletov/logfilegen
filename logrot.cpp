#include "logrot.h"

CLogRotator::CLogRotator (const string &fname, size_t maxfiles, size_t maxfilesize)
{
  source_filename = fname;
  max_log_files = maxfiles;
  max_log_file_size = maxfilesize;

}
