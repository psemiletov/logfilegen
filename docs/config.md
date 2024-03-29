## Configuration

logfilegen is controlled by the configuration variables. They can be set in a three
complementary ways and can be defined via the configuration file, the command line, and environment variables. The overriding order is:

1. Hardcoded default values

2. Configuration file

3. ```--config=filename``` (override the configuration file)

4. Command line

5. Environment variables


## Variables


You can use following configuration variables:

### Process workflow

**duration=integer** - how many seconds runs the lines gerenation cycle. If 0 (zero), cycle will run until break by Ctrl-C.

**rate=integer** - how many lines we generate at the each cycle iteration. If ```rate``` = 0, logfilegen performs non-timed loop with the full processor power. Thus, we have two main modes - *rated* (rate=non-zero) and *unnrated* (rate=0). Default: 0.

**lines=integer** - overrides ```duration``` with the ```lines``` count to generate exact number of the lines. Maximum lines count is 18446744073709551615.

**size=integer** - overrided ```duration``` and ```lines``` to produce the log file with ```size``` content. Default units is bytes. You can use the suffix "k" to define kilobytes, "m" for Mb's, and "g" for Gb's, i.e.: 64k, 16m, etc.

**template=string** - file name of the template that is used for logfile lines generation. (See [Using templates](templates.md) chapter).

**logfile=string** - file name for the resulting logfile. If no absolute file provided, the program will search in the current directory. You can also use ```stdout``` as the file name to output lines to the console. It is slow, but good way for the quick testing.

**threads=integer** - use multiply threads (*unrated* mode only). Default: 2 (1 on single core). The optimal value can be found by experiments, because there are many factors (disk IO, log rotationn, etc) those prevents many threads utilization. 3-4 threads may increase the performance on 70-80 percents, but further (mor than 4 threads) we may expect the significant degradation of the performance.


### Log rotation

**logcount=integer** - maximum log files used in log rotation.

**logsize=integer** - maximum log file size (in bytes by default). If exeeds, the log rotation will happen. The value can be with the suffix "k" to set the kilobytes and "m" for the megabytes, "g" for Gb. The default value is ```16m```.

**gzip=boolean** - if true, use external gzip to compress "rotated" log files.


### Process benchmark

**pure=boolean** - "true" or "false" (default). It that flag is "true", logfilegen just generate log lines at the memory without the actual file output.

**test=boolean** - "true" or "false" (default). A some sort of quick benchmark at full speed, using the default template for the current mode (nginx by default), one step of the log rotation; output to the temporary file at system's temporary directory (the output log will be deleted after all is done). Result, in lines per second, may vary depended on the randomizer engine work and use of gzip.

**benchmark=boolean** - "true" or "false" (default). It "true", logfilegen run the generation at full speed, with the current template (or the default one if not provided), to the current log file. Result, in lines per second, may vary depended on the template complexity, randomizer engine work, use of gzip, log rotation settings. Use ```benchmark``` instead of ```benchmark``` when you want to know the performance of some working template and config, on the given filesystem (```temp``` directory from ```test``` may have the performance different from the other disks or partitions).


### Process info

**debug=boolean** - if true, show debug messages. Be verbose, dude! False by default.

**metrics=boolean** - expose metrics. See [Using metrics](metrics.md) for a details.

**port=number** - redefines the default 8080 metrics port.

**poll=seconds** - set in seconds the auto-update interval of built-in info web page (localhost:8080/)


### Post-run info

**results=filename** - write results when job is done, to the filename. filename MUST be
the absolute (with the full path) or "stdout" (without quotes).

**results_template=strint** - set the format string for the option above. The default is "@date - @duration - @mode/@template - @size_generated - @lines_generated - @performance_lps". You can also use @performance_bps to indicate bytes per second. (NEED TO EXPLAIN MORE!)


## logfilegen.conf

The configuration file (where the variables can be defined) is called ```logfilegen.conf``` and must be placed to ```/etc/logfilegen/``` or ```$HOME/.config/logfilegen/``` or to current the directory (where logfilegen binary has been runned).

But you can use any config file using ```--config=filename``` option.

logfilegen config file is a simple variable=value text file.

Example:

```
duration=2
rate=5
logfile=stdout
template=example-nginx-test.tp
#template=/home/test/test01.tp
```

In this example, we run logfilgen for a 2 seconds, generating 5 lines per second, to the screen (stdout), using ```example-nginx-test.tp``` user-written template. Please note that we can "comment-out" lines with ```#``` to disable them.


## Command line parameters

All configuration variables from the configuration file can be **overriden** using command line options in the format ```--key=value```. For example:

```
logfilegen --duration=3
```

You can mix config file and command line options, as good as with Environment variables (See below). For boolean variables, the explicitly stated key means "true", i.e.:

```
logfilegen --benchmark
```

But, in the case of the configuration file or environment variable, you need to set to true or false


## Environment variables

All variables can be also set via enviromnent variables using ```export KEY=VALUE```, with the UPPER-CASED key name.

The usual variable name must be prefixed with ```LFG_``` (shortened from logfilegen), for example **LFG_DURATION** sets **duration** variable.

Example:

```export LFG_DURATION=20```


## Examples

Use the way that you prefere - configuration file, command line parameters or the environment. Please not that some parameters are compatible one with other, and some are not, i.e. ```lines``` disables ```duration```, and ```size``` disabled ```lines```. That's the only rule.


### Example 000

Know your system:

```
./logfilegen --benchmark
```

That show how many lines per second can generate logfilegen with a current settings (other parameters, config variables, etc.). So you will know the maximum generation rate that logfilegen gains.



### Example 001

The config file to use default (built-in) template for nginx, generate lines at full speed, 20 seconds, to ```/home/test/out.log``` using ```test.tp```.


```
duration=20
logfile=/home/test/out.log
template=test.tp
```

And ```test.tp``` can be, for example:


```
$test=@str:1:10|world
$logstring=Hello, $test
```

Here, at each ```$logstring``` generation iteration, it will be ```Hello, world``` or ```Hello, some random word```.



### Example 002


Generate 20000 lines per second, 10 seconds, with the default (built-in) nginx template, then stop:


```
logfilegen --duration=10 --rate=20000
```

### Example 003


Generate exactly 100000 with rate 20000 lines per second, with the default (built-in) nginx template, then stop:


```
logfilegen --lines=100000 --rate=20000
```

### Example 004


Generate 64 Mb lines, at full speed, with the default (built-in) nginx template, making log rotation using 4 log filesm each 1 Mb size:


```
logfilegen --size=64M --logcount=4 --logsize=1M
```

### Example 005

Generate lines to stdout (console), in the infinity loop, and rate 10 lines per second, until breaked by Ctrl-C:


```
logfilegen --rate=10 --logfile=stdout
```



## Read next - Templates, Metrics

[Templates manual](templates.md)

[Using metrics](metrics.md)



