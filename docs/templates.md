## Templates

The generating log file content is defined by the template file. You can have several template files of different purposes.

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

We define the variable ```$test``` with the value ```hello```, the variable ```test2``` with ```world``` value, set ```$remote_addr``` to ```1111.1111.1111.1111``` and use these variables at the value of ```$logstring```.

The value of the variable can be:

- text (```hello```)

- fractional number with a fixed point (```1.0001```)

- range of integer numbers (```$var=1..1111111```)

- sequence (```$var=APPLE|2000|FRUITS|HELLO|POTATO```)

- macros (```@macros:value1:value2```). Note! Using of macros directly in ```$logstring``` is possible but not desirable.


#### text value

The **text** value can be any text - characters, spaces, numbers, etc.

Syntax: ```$variable=value```

Example:


```
$test=world
$logstring=hello, $test!
```

The output string will be ```hello, world!```.


#### fractional number with a fixed point

Used in a rare cases, to generate random seconds-like value with some digits after the fixed point.

Syntax: ```$variable=min_seconds.mantissa..max_seconds.mantissa```

We set the minimum and maximum generating values. The digits in mantissa defines the precision after the fixed point. For example, how we define the value that can vary from 0 to 60 seconds with msecs precision:

```
$seconds_random=0.001..59.000
$logstring=$seconds_random
```

#### range of integer numbers

The value can vary randomly from the minimum to the maximum range limits.

Syntax: ```$variable=min..max```

Example:

```
$status=400..451
$logstring=status is $status
```

At the each log string generating iteration, ```$status``` will be random number within 400 to 451 range.


#### sequence

The sequence is a set of values, delimeted by ```|```. Each value can be choosed randomly at each variable use during the logstring generation.

Format: ```$variable=value 1|value 2|etc|```

Example:

```
$status=403|404|502|200
$logstring=status is $status
```

When processed, $status value will be 403 or 404 or 502 or 200 randomly.


#### macros

Macros are programmatically-generated values with a special names and parameters (optional or mandatory). Parameters are delimited by the colons (":"). Macros must be used as **variable values**, not as the elements of the ```$logstring``` directly.

Here are a list of logfilegen macros:

##### @datetime

Current date and time stamp with a given [strftime-based format](https://en.cppreference.com/w/c/chrono/strftime).

Syntax: ```$variable=@datetime:format```

Example:

```
$timestamp=@datetime:%d/%b/%Y:%H:%M:%S %z
$logstring=how is $timestamp
```

Here we set the standard date-time format of **nginx** log.

##### @str_random

Generates a random string with a given length.

Syntax: ```$variable=@str_random:length``` or ```$variable=@str_random:min:max```

Example. Here we generate a random string of 8 characters:

```
$test=@str_random:8
$logstring=hello, $test
```

Example. Here we generate a random string with the length from 8 to 16 characters:

```
$test=@str_random:8:16
$logstring=hello, $test
```

##### @int_random

Generates a random integer number with a given length.

Syntax: ```$variable=@int_random:length``` or ```$variable=@int_random:min:max```

Example. Here we generate a random humber of 8 digits:

```
$test=@int_random:8
$logstring=hello, $test
```

Example. Here we generate a random digits with the length from 8 to 16 digits:

```
$test=@int_random:8:16
$logstring=hello, $test
```


##### @ip_random

Generates the random IP address.

Syntax: ```$variable=@ip_random```

Example:

```
$test=@ip_random
$logstring=hello, $test
```


##### @str_path

Generates the random path with a given minimum and maximum length of each path **element**, and the maximum subdirectories depths.

Syntax: ```@str_path:min:max:depth```

Example:

```
$testpath=@str_path:1:10:3
$logstring=The paths is $testpath
```

Were, macro ```@str_path``` expands to the random-generated path with the path parts length randomly varied from 1 to 10, and depth from 1 to 3



##### $file_source

Acts as the text file loader for **sequence** variable values.

Syntax: ```@file_source:full path to file```

Consider we have the template like this:

```
$groups=Beatles|Nirvana|Radiohead
$logstring=hello, $groups!
```

Here we defined the sequence of values, separated by ``|``. But what if we want to have large list of such values, dozens or hundreds? In this case, use ```$file_source``` variable (with the **full file path**). Edit our example to something like that:

```
$groups=@file_source:/home/test/testsource.txt
$logstring=hello, $groups!
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

When processed, the ```@file_source:/home/test/testsource.txt``` directive will load ```/home/test/testsource.txt```, transform it to ```|```-separated values and choose one of them randomly.





### Complex examples

#### Example 01

```
$remote_addr=@ip_random
$remote_user=@str_random:4:12|@int_random:16
$time_local=%x:%X
$request=POST|GET
$status=200|404
$body_bytes_sent=100..10000
$time_iso8601=%Y-%m-%dT%H:%M:%SZ
$logstring=$remote_addr - $remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent"
```

