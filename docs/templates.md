## Templates

The generating log file content is defined by the templates file.

Template is a plain text file with key=value lines. Each key corresonds to variable of some web server (nginx support is currently implemented and used by default). To use the template file, use ```--template=filename``` option. If no absolute path given, the file must be placed to one of the following locations: the current directory, ```./templates```, ```$HOME/.config/logfilegen/templates```, or ```/etc/logfilegen/templates```. logfilegen will use the first founded template with a given filename, so the template ```test.tp``` in the current directory overrides ```./templates/test.tp```, etc.

Example of such file:

```
$remote_addr=@ip_random
$remote_user=@str_random:4:12|@int_random:22
$time_local=@datetime:%x:%X
$request=GET|POST|PUT|PATCH|DELETE
$status=200|404
$body_bytes_sent=100..10000
$logstring=$remote_addr - $remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent"
```

As you see, ```$logstring``` is the variable which value is a free form string defining the log output string. Here we can mix the usual text and variables.

The variables can be pre-defined in the server (such as ```$status```, ```$body_bytes_sent```, etc) or defined by the user. All pre-defined variables can be overrided at the template file.

Currently logfilegen "knows" the following [standard nginx variables](http://nginx.org/en/docs/varindex.html) variables: ```$body_bytes_sent```, ```$connection_time```, ```$document_uri```, ```$http_referer```, ```$http_user_agent```, ```$protocol```, ```$remote_addr```, ```$remote_user```, ```$request```, ```$request_time```, ```$status```, ```$time_iso8601```, ```$time_local```, ```$uri```.


There are also special logfilegen variables ```$seconds_random``` (WRITE ABOUT IT!!!!) and ```$logstring```.

To redefine the "built-in" variable use just need to set new value to it. To define a new variable you need do the same. Example of the small template file:

```
$test=hello
$test2=world
$remote_addr=1111.1111.1111.1111
$logstring=Hi! $test $test2 $remote_addr
```

As a result, logfilegen will generate to the log file the following line: ```Hi! hello world 1111.1111.1111.1111```.

We define the variable ```$test``` with the value ```hello```, the variable ```test2``` with ```world``` value, set ```$remote_addr``` to ```1111.1111.1111.1111``` and use that macros at the value of ```$logstring```.

The value of the variable can be:

- string (```hello```)

- integer number (```777```)

- real number with a fixed point (```1.0001```)

date and time (```%d%b%Y:%H:%M:%S %z```)

range of integer numbers (```$var=1..1111111```)

sequence of strings (```$var=APPLE|2000|FRUITS|HELLO|POTATO```)

macros (```@macros:value1:value2```). Note! Using of macros directly in ```$logstring``` is possible but not desirable.





## Examples


```
$test=@str_random:2
$test2=@str_random:2|@int_random:4
$test3=@str_random:4:12
$logstring=hello $test $test2 $test3 world
```

```
$test2=@str_random:2|@int_random:4:6
$logstring=hello $test2 world
```

## OLD TEXT - TO REWRITE!!!


In macros, we can use **ranges** (i.e. ```1..1111111```) and **sequences** (```1|3|6|888|HELLO|WORLD```). The ranged value means that macro will be replaced with the randomly taken value within the range. The sequences is the set of values, where one of them will be choosen randomly.

Each variable can be uses **one time** per line, i.e. there is no ```$status foobar $status``` support.

Among standard nginx variables, you can define your own, for example:

```
$time_local=%x:%X
$request=POST
$status=200|404
$body_bytes_sent=100..10000
$testvar=HELLO|WORLD
$time_iso8601=%Y-%m-%dT%H:%M:%SZ
$logstring=$time_iso8601 $testvar $remote_addr - $remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent"
```

Here we define two custom variables - ``$testvar`` that can be "HELLO" or "WORLD" on each log generation iteration, and ```$time_iso8601``` with ```%Y-%m-%dT%H:%M:%SZ``` date time format.

In the template you can define "randomization" variable for the numbers and strings with a given length. Such variable values can use ```$int_random``` and ```$str_random``` macros, with the length of the sequence to generate. Example:

```
$test=@str_random:10
$logstring=hello $test world
```

So, specify the length of the generated word after the ":". I.e. ```@int_random:5``` or ```@str_random:11```


To define timestamp variable with [strftime-based](https://en.cppreference.com/w/c/chrono/strftime) format, use ```@datetime``` macro with syntax: ```@datetime:format```. Example of the template that uses ```@datetime```:

```
$test=$datetime:%d-%b-%Y %H:%M:%S %z
$logstring=hello $test world
```

logfilegen can deal with seconds-based fixed point variables with msecs resolution such as ```$request_time```, ```$connection_time``` (and special ```$seconds_random```). They can be redefined or used in a following way:

```
$test=1.0..60.0
$logstring=world $request_time $status $test hello $seconds_random
```

Fixed point values must contain the dot symbol (.) as the fixed point. The precision is controlled by the number of digits after the fixed point, i.e. 1.0000 has the precision 4.


### Template macros


- **@ip_random** - generate a random ip.

Example:

```
$logstring=@datetime:%d%b%Y:%H:%M:%S %z @ip_random
```


**$file_source**

```@file_source:full path to file``` - acts as the text file loader for **sequence** variable values. Consider we have the template like this:

```
$groups=Beatles|Nirvana|Radiohead
$logstring=hello $groups world
```

Here we defined the sequence of values, separated by "|". But what if we want to have large list of such values, dozens or hundreds? In this case, use ```$file_source``` variable (with the **full path to the file**). Edit our example to something like that:

```
$groups=@file_source:/home/test/testsource.txt
$logstring=hello $groups world
```

And create the ```/home/test/testsource.txt``` just with the plan text content, line by line (with new line at the end), for example:

```
Beatles
Radiohead
Nirvana
Pixies
Frank Black
Skinny Puppy
```

When processed, the ```@file_source:/home/test/testsource.txt``` directive will load ```/home/test/testsource.txt```, transform to "|"-separated values and choose one of them randomly.


**$str_path**

```@str_path:min:max:deep```, let'e show the example of the template:

```
$testpath=@str_path:1:10:3
$logstring=hello 4testpath world
```

Macro ```@str_path``` expands to the random-generated path with the path parts length randomly varied from ```min``` to ```max```, using ```deep``` subdirectories.


