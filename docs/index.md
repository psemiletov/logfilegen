# logfilegen

Logfilegen is a fast and highly customizable tool to generate common server (nginx, etc) or user-defined format log files. You can generate log file with the desired rate (lines per second), the file size, lines count and the duration. Each variable of the log file can be redefined by the random or static value. logfilegen depends just on GCC or Clang with C++11 support, and, optionally, cmake.

## LINKS

[Github repo](https://github.com/psemiletov/logfilegen), [Latest release](https://github.com/psemiletov/logfilegen/releases/latest)

[Dockerhub repo](https://hub.docker.com/r/psemiletov/logfilegen/general)

[AUR](https://aur.archlinux.org/packages/logfilegen), [openSUSE](https://software.opensuse.org/package/logfilegen)


## NEWS

***02 March 2023, 2.3.0***

+ CMake support optimization

+ HaikuOS support (compile with make --makefile=Makefile.Haiku

+ Mac ARM support (compile with make --makefile=Makefile.macarm

* randomization engine take less resources




## MANUAL

To install it, please read [Installation guide](inst.md)

logfilegen is controlled by the configuration variables and templates. The usual way to run logfilegen from the command line is, for example:


```console
logfilegen --template=mytemplate.tp --duration=20 --rate=100 --logfile=out.log
```

The **configuration variables** affects how logfilegen works, and **templates** affects the  generation of log file. There are many customization ways for logfilegen. To master them, you can read the following chapters:


[Using configuration](config.md)

[Using templates](templates.md)

[Using metrics](metrics.md)

[Benchmarking](https://github.com/psemiletov/logfilegen/tree/main/benchmark) (by air3ijai)




Written by Peter Semiletov, 2023
