# logfilegen
The server log file generator

[![Release](../../actions/workflows/release.yml/badge.svg)](../../actions/workflows/release.yml)
[![Tests](../../actions/workflows/tests.yml/badge.svg)](../../actions/workflows/tests.yml)

[![Docker](../../actions/workflows/docker.yml/badge.svg)](../../actions/workflows/docker.yml)
[![Docker downloads](https://img.shields.io/docker/pulls/psemiletov/logfilegen.svg)](https://hub.docker.com/r/psemiletov/logfilegen)

[Dockerhub repo](https://hub.docker.com/r/psemiletov/logfilegen/general)

[AUR](https://aur.archlinux.org/packages/logfilegen)


THIS SOFTWARE IS IN ALPHA STATE!!!!

## PREFACE

All that is documented is work.


## INSTALLATION

To compile logfilegen from the source you need GCC/g++ or Clang with C++11 version support. If you did not compiled programs before, install g++ or Clang, and **make** utility to your system. Then, in the simplest case, run as root or with sudo, at the logfilegen source directory:


```console
make
make install
```

And to uninstall:


```console
make uninstall
```


## USE

logfilegen is controlled by the configuration variables and templates. The usual way to run logfilegen from the command line is, for example:


```console
logfilegen --duration=20 --rate=100 --logfile=out.log
```

All parameters are inner variables that can be set in various ways.


## Configuration variables

The configuration variables defines how work logfilegen when run. They can be defined via the configuration file, the command line, and environment variables. The overriding order is:

1. Hardcoded default values

2. Configuration file

3. Command line

4. Environment variables


You can use following configuration variables:

**pure=boolean** - "true" or "false" (default). It "true" we just generate log strings in the memory without the actual file output.

**duration=integer** - how many seconds runs the lines gerenation cycle. If 0 (zero), cycle will run until break by Ctrl-C

**rate=integer** - how many lines we generate at the each cycle iteration. If ```rate``` = 0, logfilegen performs non-timed loop with the full processor power.

**lines=integer** - overrides ```duration``` with the ```lines``` count to generate exact number of the lines. Maximum lines count is 18446744073709551615.

**size=integer** - overrided ```duration``` and ```lines``` to produce the log file with ```size``` content.

**templatefile=string** - file name of the template that is used for logfile lines generation. (See Templates section)

**logfile=string** - file name for the resulting logfile. If no absolute file provided, the program will search in the current directory. You can also use "stdout" (without the quotes) as the file name to output lines to the console.

**logcount=integer** - maximum log files used in log rotation.

**logsize=integer** - maximum log file size (in bytes by default). If exeeds, the log rotation will happen. The value can be with the suffix "k" to set the kilobytes and "m" for the megabytes, "g" for Gb, i.e. ```64k``` or ```16m```. The default value is ```16m```.

**debug=boolean** - if true, show debug messages. Be verbose, dude! False by default.

**gzip=boolean** - if true, use external gzip to compress "rotated" log files


### logfilegen.conf

The configuration file is called ```logfilegen.conf``` and must be placed to ```/etc/logfilegen/``` or ```$HOME/.config/logfilegen/``` or to current the directory (where logfilegen binary has been runned).

```logfilegen.conf``` is a simple key=value text file.

Example:

```
duration=2
rate=5
logfile=stdout
debug=false
templatefile=example-nginx-test.tp
#templatefile=t01.tp
#templatefile=/home/test/test01.tp
pure=false
```

Please note that you can "comment-out" lines to disable them.


### Command line parameters

All configuration variables can be **overriden** using command line options in the format ```---key=value```. For example:

```console
logfilegen --duration=3
```


### Environment variables

All variables above can be also set via enviromnent variables using ```export KEY=VALUE```

**LFG_DURATION** - set **duration** variable

**LFG_RATE** - set **rate**

**LFG_TEMPLATEFILE** - set **templatefile**

**LFG_LOGFILE** - set **logfile**

**LFG_LOGSIZE** - set **logsize**

**LFG_LOGCOUNT** - set **logcount**

**LFG_GZIP** - set **gzip**


Example:

```export LFG_DURATION=20```


## Templates

See [Templates manual](/docs/templates.md)
