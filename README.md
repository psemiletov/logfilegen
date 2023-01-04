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

As you see, all variables (except $logstring) is the standard nginx veriables (see http://nginx.org/en/docs/varindex.html), and here we can assign macros for them.

The special variable is **$logstring**, it hold the free from text template of the logging string. The default one is seen above.

In macros, we can use ranges (i.e. 1..1111111) and sequences (1|3|6|888|HELLO|WORLD). The ranged value means that macro will be replaced with the randomly taken value within the range. The sequences is the set of values, where one of them will be choosen randomly.

