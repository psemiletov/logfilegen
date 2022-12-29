/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/


#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <algorithm>


#include "libretta_pairfile.h"

using namespace std;

typedef std::map <string, string> TSPair;


void CPairFile::save()
{
  ofstream myfile (file_name.c_str());
  if (! myfile.is_open())
      return; 
 
  TSPair::const_iterator end = values.end(); 

  for (TSPair::const_iterator it = values.begin(); it != end; it++)
     {
      myfile << it->first << "=" << it->second << '\n';
     }
  
  myfile.close();
}


void CPairFile::set_string (const string &key, const string &value)
{
  values[key] = value; 
}


void CPairFile::set_float (const string &key, float value)
{
  char buffer [64];
  sprintf (buffer, "%f", value);
  string n (buffer);
  values[key] = n; 
}


float CPairFile::get_float (const string &key, float def_value)
{
  float result = def_value; 

  if (values[key].size() == 0)
     return result; 
  
  const char *s = values[key].c_str();
  if (s)
     result = atof (s);
   
  return result;
}


void CPairFile::set_int (const string &key, int value)
{
  char buffer [64];
  sprintf (buffer, "%d", value);
  string n (buffer);
  values[key] = n; 
}


int CPairFile::get_int (const string &key, int def_value)
{
  int result = def_value; 
  
  if (values[key].size() == 0)
     return result; 
  
  const char *s = values[key].c_str();
  if (s)
     result = atoi (s);
   
  return result;
}


string CPairFile::get_string (const string &key, 
                              const string &def_value)
{
  string result = def_value; 

  if (values[key].size() == 0)
     return result; 
 
  string s = values[key];
  if (! s.empty())
     result = s;
   
  return result;
}


CPairFile::CPairFile (const string &fname, bool from_data)
{
  if (from_data)
    {
     stringstream st (fname);
     string line;
     
     while (getline (st, line)) 
           {
            if (line.empty())
               continue;
           
            size_t pos = line.find ("=");
                 
            if (pos == string::npos)
                continue;
        
            if (pos > line.size())
                continue;
                    
            string a = line.substr (0, pos);
            string b = line.substr (pos + 1, line.size() - pos);

            //cout << a << ":" << b << endl;
            
            values[a] = b;
           }
     
     return; 
    }
 
 
  file_name = fname;
  
  ifstream infile (file_name.c_str());

  if (infile.fail())
     {
      cout << "CPairFile::CPairFile - Could not open file: " << fname << endl;
      return;
     }

  string line;

  while (getline (infile, line))
        {
         size_t pos = line.find ("=");
         
         if (pos == string::npos)
            continue;
        
         string a = line.substr (0, pos);
         string b = line.substr (pos + 1, line.size() - pos);

         values[a] = b;
        }

  infile.close();
};


CPairFile::CPairFile (int argc, char *argv[])
{
  stringstream st;

  for (int i = 0; i < argc; i++)
     {
      st << argv[i];
      st << endl;
     }

  string line;

  while (getline (st, line))
        {
         if (line.empty())
            continue;

         size_t pos = line.find ("=");

         if (pos == string::npos)
             continue;

         if (pos > line.size())
             continue;

         string a = line.substr (0, pos);
         a.erase (0,2);

         cout << "key:" << a << endl;

         string b = line.substr (pos + 1, line.size() - pos);
         values[a] = b;
        }

}


CPairFile::CPairFile (vector <string> envars)
{
 cout << "CPairFile::CPairFile (vector <string> envars)" << endl;


  stringstream st;


  for (size_t i = 0; i < envars.size(); i++)
      {
       const char* env_p = std::getenv(envars[i].c_str());
       if (env_p)
          {
           cout << "env_p: " << env_p << endl;

           string param = envars[i];
           param.erase (0, 4);

//           std::transform (param.begin(), param.end(), param.begin(), std::tolower);
    std::for_each(
    param.begin(),
    param.end(),
    [](char & c) {
        c = ::tolower(c);
    });


           string value = env_p;

           //cout << "envars[" << i << "] = " << t << endl;

           cout << param << "=" << value << endl;


           st << param << "=" << value << endl;


          //st << endl;

         }

     }


   cout << "END ---  CPairFile::CPairFile (vector <string> envars)" << endl;

  string line;

  while (getline (st, line))
        {
         if (line.empty())
            continue;

         size_t pos = line.find ("=");

         if (pos == string::npos)
             continue;

         if (pos > line.size())
             continue;

         string a = line.substr (0, pos);

         string b = line.substr (pos + 1, line.size() - pos);
         values[a] = b;
        }

}


