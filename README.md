# logfilegen
The server log file generator

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

**pure=bool** - "true" or "false" (default). It "true" we just generate log strings in the memory without the actual file output.

**duration=integer** - how many seconds runs the lines gerenation cycle.

**rate=integer** - how many lines we generate at the each cycle iteration.

**templatefile=string** - file name of the template that is used for logfile lines generation. (See Templates section)

**logfile** - file name for the resulting logfile. If no absolute file provided, the program will search in the current directory. You can also use "stdout" (without the quotes) as the file name to output lines to the console.



### logfilegen.conf

The configuration file is called ```logfilegen.conf``` and must be placed to ```/etc/logfilegen/``` or ```$HOME/.config/logfilegen/``` or to current the directory (where logfilegen binary has been runned).

```logfilegen.conf``` is a simple key=value text file.




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


Example:

```export LFG_DURATION=20```


## Templates


Each template file is a simple key=value file, which must be placed to one of the following locations:

```
./templates
```

Or


```/etc/logfilegen/templates```


Or


```$HOME/.config/logfilegen/templates```


Example of such file:

```
$remote_addr=IP_RANDOM
$remote_user=WORD|NUMBER
$time_local=%x:%X
$request=GET|POST|PUT|PATCH|DELETE
$status=200|404
$body_bytes_sent=100..10000
$logstring=$remote_addr - $remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent"
```

As you see, all variables (except ```$logstring``` and special ```type_random``` variables) is the [standard nginx veriables](http://nginx.org/en/docs/varindex.html), and here we can assign macros for them.

The special variable is ```$logstring```, it hold the free from text template of the logging string. The default one is seen above.

The following variables have built-in (but redefinable) values: ```$body_bytes_sent```, ```$logstring```, ```$remote_addr```, ```$remote_user```, ```$request```, ```$status```, ```$time_local```, ```$http_referer```, ```$http_user_agent```. The ```$remote_user``` variable can hold any string or macros ```USER_WORD``` or ```USER_NUMBER``` (the last two will be substituted with random-generated string or number).

In macros, we can use ranges (i.e. ```1..1111111```) and sequences (```1|3|6|888|HELLO|WORLD```). The ranged value means that macro will be replaced with the randomly taken value within the range. The sequences is the set of values, where one of them will be choosen randomly.

Each variable can be uses **one time** per line, i.e. there is no ```$status foobar $status``` support.

Among standard nginx variables, you can define your own, for example:

```
$remote_addr=IP_RANDOM
$remote_user=WORD|NUMBER
$time_local=%x:%X
$request=POST
$status=200|404
$body_bytes_sent=100..10000
$testvar=HELLO|WORLD
$time_iso8601=%Y-%m-%dT%H:%M:%SZ
$logstring=$time_iso8601 $testvar $remote_addr - $remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent"
```

Here we define two custom variables - ``$testvar`` that can be "HELLO" or "WORLD" on each log generation iteration, and ```$time_iso8601``` with ```%Y-%m-%dT%H:%M:%SZ``` date time format.

In the template you can define "randomization" variable for the numbers and strings with a given length. Such variable names must begin with ```$int_random``` or ```$str_random```, and the value is the length of the sequence to generate. Example:

```
$testvar=HELLO|WORLD
$time_iso8601=%Y-%m-%dT%H:%M:%SZ
$str_random01=6
$int_random01=12
$logstring=$time_iso8601 $testvar $remote_addr - $remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent" $str_random01 test test $int_random01
```

All variables that name starts from ``$time_`` logfilegen handles as date time variables and their values must contain date/time format string. For nginx, use [strftime format](https://en.cppreference.com/w/c/chrono/strftime)
