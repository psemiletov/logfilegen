# logfilegen
The server log file generator

THIS SOFTWARE IS IN ALPHA STATE!!!!

## PREFACE

All that is documented is work.

logfilegen is controlled by the configuration variables and templates.

## Configuration variables

The configuration variables defines how work logfilegen when run. They can be defined via the configuratiuon file, the command line, and environment variables. The overriding order is:

1. Hardcoded default values

2. Configuration file

3. Command line

4. Environment variables


You can use following configuration variables:

**duration=integer value** - how many seconds runs the lines gerenation cycle

**rate=integer value** - how many lines we generate at each cycle iteration

**templatefile=string value** - file name of the template that is used for logfile record generation. (See Templates section)





### logfilegen.conf

The configuration file is called ```logfilegen.conf``` and must be placed to ```/etc/logfilegen/``` or ```$HOME/.config/logfilegen/``` or to current the directory (where logfilegen binary has been runned).

```logfilegen.conf``` is a simple key=value text file.





### Command line parameters

All configuration variables can be **overrided** using command line options in the format ```---key=value```. For example:

```console
logfilegen --duration=3
```



### Environment variables

All variables above can be also set via enviromnent variables using ```export KEY=VALUE```

**LFG_DURATION** - set **duration** variable

**LFG_RATE** - set **rate**

**LFG_TEMPLATEFILE** - set **templatefile**

Example:

```export LFG_DURATION=20```


## Templates

