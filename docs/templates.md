## Templates

The generating log file content is defined by the templates file.

Template is a plain text file with key=value lines. Each key corresonds to variable of some web server (nginx support is currently implemented and used by default). To use the template file, use ```--template=filename``` option. If no absolute path given, must be placed to one of the following locations: the current directory, ```./templates```, ```$HOME/.config/logfilegen/templates```, ```/etc/logfilegen/templates```. logfilegen will use the first found template with a given filename, so the template ```test.tp``` in the current directory overrides ```./templates/test.tp```, etc.

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


```
$test=@str_random:2
$test2=@str_random:2|@int_random:4
$test3=@str_random:4:12
$logstring=hello $test $test2 $test3 world
```

$test2=@str_random:2|@int_random:4:6
$logstring=hello $test2 world
