## Templates

The generating log file content is defined by the template file. You can have several template files of different purposes.

Template is a plain text file with key=value lines. Each key corresonds to variable of some web server (nginx support is currently implemented and used by default). To use the template file, use ```--template=filename``` option. If no absolute path given, the file must be placed to one of the following locations: the current directory, ```./templates```, ```$HOME/.config/logfilegen/templates```, or ```/etc/logfilegen/templates```. logfilegen will use the first founded template with a given filename, so the template ```test.tp``` in the current directory overrides ```./templates/test.tp```, etc.

Example of such file:

```
$remote_addr=@ip
$remote_user=@str:4:12|@int:22
$time_local=@datetime:%x:%X
$request=GET|POST|PUT|PATCH|DELETE
$status=200|404
$body_bytes_sent=100..10000
$logstring=$remote_addr - $remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent"
```

As you see, ```$logstring``` is the variable which value is a free form string defining the log output string. Here we can mix the usual text and variables. If no ```$logstring``` provided at the template, the default built-in template (for the current mode, i.e. nginx, apache, etc.) will be used.

The variables can be pre-defined in the server itself (such as ```$status```, ```$body_bytes_sent``` for nginx, etc) or defined by the user. All pre-defined variables can be overrided at the template file.

Currently logfilegen "knows" the following [standard nginx variables](http://nginx.org/en/docs/varindex.html) variables: ```$body_bytes_sent```, ```$connection_time```, ```$document_uri```, ```$https```, ```$http_referer```, ```$http_user_agent```, ```$is_args```, ```$pipe```, ```$protocol```, ```$remote_addr```, ```$remote_user```, ```$request```, ```$request_id```, ```$request_completion```, ```$request_time```, ```$scheme```, ```$status```, ```$time_iso8601```, ```$time_local```, ```$uri```.


There are also special logfilegen variable ```$logstring``` for all logfilegen modes.

To redefine the "built-in" variable use just need to set new value to it. To define a new variable you need do the same. Example of the small template file:

```
$test=hello
$test2=world
$remote_addr=1111.1111.1111.1111
$logstring=Hi! $test $test2 $remote_addr
```

As a result, logfilegen will generate to the log file the following line: ```Hi! hello world 1111.1111.1111.1111```.

We define the variable ```$test``` with the value ```hello```, the variable ```test2``` with ```world``` value, set ```$remote_addr``` to ```1111.1111.1111.1111``` and use these variables at the value of ```$logstring```.

The value of the variable can be:

- text (```hello```)

- fractional number with a fixed point (```1.0001```)

- range of integer numbers (```$var=1..1111111```)

- sequence (```$var=APPLE|2000|FRUITS|HELLO|POTATO```)

- macros (```@macros:value1:value2```). Note! Using of macros directly in ```$logstring``` is not supported! Macros can be used exclusively in variable values.


### text value

The **text** value can be any text - characters, spaces, numbers, etc.

Syntax: ```$variable=value```

Example:


```
$test=world
$logstring=hello, $test!
```

The output string will be ```hello, world!```.


### fractional number with a fixed point

Used in a rare cases, to generate random seconds-like value with some digits after the fixed point.

Syntax: ```$variable=min_seconds.mantissa..max_seconds.mantissa```

We set the minimum and maximum generating values. The digits in mantissa defines the precision after the fixed point. For example, how we define the value that can vary from 0 to 60 seconds with msecs precision:

```
$seconds_random=0.001..59.000
$logstring=$seconds_random
```

### range of integer numbers

The value can vary randomly from the minimum to the maximum range limits.

Syntax: ```$variable=min..max```

Example:

```
$status=400..451
$logstring=status is $status
```

At the each log string generating iteration, ```$status``` will be random number within 400 to 451 range.


### sequence

The sequence is a set of values, delimeted by ```|```. Each value can be choosed randomly at each variable use during the logstring generation. At the sequence you can mix plain values and macros.

Format: ```$variable=value 1|value 2|etc|```

Example:

```
$status=403|404|502|200
$logstring=status is $status
```

When processed, $status value will be 403 or 404 or 502 or 200 randomly.

Example 2:

```
$test=@str:8|world
$logstring=Hello, $test!
```


### macros

Macros are the generated values with special names and parameters (optional or mandatory). Parameters are delimited by the colons (``:``). Macros must be used as **variable values**, not as the elements of the ```$logstring``` directly.

Here are a list of logfilegen macros:

#### @datetime

Current date and time stamp with a given [strftime-based format](https://en.cppreference.com/w/c/chrono/strftime).

Syntax: ```$variable=@datetime:format```

Example:

```
$timestamp=@datetime:%d/%b/%Y:%H:%M:%S %z
$logstring=how is $timestamp
```

Here we set the standard date-time format of **nginx** log.


#### @str

Generates a random string with a given length.

Syntax: ```$variable=@str:length``` or ```$variable=@str:min:max```

Example. Here we generate a random string of 8 characters:

```
$test=@str:8
$logstring=hello, $test
```

Example. Here we generate a random string with the length from 8 to 16 characters:

```
$test=@str:8:16
$logstring=hello, $test
```


#### @int

Generates a random integer number with a given length.

Syntax: ```$variable=@int:length``` or ```$variable=@int:min:max```

Example. Here we generate a random humber of 8 digits:

```
$test=@int:8
$logstring=hello, $test
```

Example. Here we generate a random digits with the length from 8 to 16 digits:

```
$test=@int:8:16
$logstring=hello, $test
```


#### @hex

Generates a random hexodecimal number with a given (in decimal) length.

Syntax: ```$variable=@hex:length``` or ```$variable=@hex:min:max```

Example. Here we generate a random humber of 7 digits:

```
$test=@hex:7
$logstring=hello, $test
```


#### @ip

Generates the random IP address.

Syntax: ```$variable=@ip```

Example:

```
$test=@ip
$logstring=hello, $test
```


#### @path

Generates the random path with a given minimum and maximum length of each path **element**, and the maximum subdirectories depths.

Syntax: ```@path:min:max:depth```

Example:

```
$testpath=@path:1:10:3
$logstring=The paths is $testpath
```

Where, macro ```@path``` expands to the random-generated path with the path parts length randomly varied from 1 to 10, and depth from 1 to 3


#### @file

Acts as the text file loader for **sequence** variable values.

Syntax: ```@file:full path to file```

Consider we have the template like this:

```
$groups=Beatles|Nirvana|Radiohead
$logstring=hello, $groups!
```

Here we defined the sequence of values, separated by ``|``. But what if we want to have large list of such values, dozens or hundreds? In this case, use ```@file``` variable (with the **full file path**). Edit our example to something like that:

```
$groups=@file:/home/test/testsource.txt
$logstring=hello, $groups!
```

And create the ```/home/test/testsource.txt``` just with the plan text content, line by line (with new line at the end), for example:

```
Beatles
Radiohead
Nirvana
Pixies
Frank Black
Depeche Mode
Skinny Puppy
```

When processed, the ```@file:/home/test/testsource.txt``` directive will load ```/home/test/testsource.txt```, transform it to ```|```-separated values and choose one of them randomly.

#### @seq

Works like **sequence** variable values. Each value is separated by ```:```.

Syntax: ```@seq:param1:param2:etc```

Example:

```
$test=@seq:GET:PUT
$logstring=hello, $test
```


#### @meta

The macro for compound macros, to set the line with several macros as the variable value. Each macro in such compound macros must be placed to the braces.

Syntax: ```@meta:(macro1)(macros)```

Example:

```
$test=@macro:foo (@str:12) bar (@int:12:16) (@ip)
$logstring=hello, $test
```

Such complex macro can contain the free form text among the macros. The nested @meta macros are not supported.


### Complex examples

#### Example 01

```
$remote_addr=@ip
$remote_user=@str:4:12|@int:16
$time_local=@datetime:%x:%X
$request=POST|GET
$status=200|404
$body_bytes_sent=100..10000
$time_iso8601=@datetime:%Y-%m-%dT%H:%M:%SZ
$logstring=$remote_addr - $remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent"
```
