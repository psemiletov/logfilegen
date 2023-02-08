# logfilegen

Logfilegen is a fast and highly customizable tool to generate common server (nginx, etc) or user-defined format log files. You can generate log file with the desired rate (lines per second), the file size, lines count and the duration. Each variable of the log file can be redefined by the random or static value. logfilegen depends just on GCC or Clang with C++17 support, and, optionally, cmake.

## LINKS

[Github repo](https://github.com/psemiletov/logfilegen), [Latest release](https://github.com/psemiletov/logfilegen/releases/latest)

[Dockerhub repo](https://hub.docker.com/r/psemiletov/logfilegen/general)

[AUR](https://aur.archlinux.org/packages/logfilegen), [openSUSE](https://software.opensuse.org/package/logfilegen)


## NEWS

***08 february 2023, 1.2.0*** - This release adds a new, simplifier version of the benchmark: --test

A some sort of quick benchmark at full speed, using the default template for the current mode (nginx by default), one step of the log rotation; output to the temporary file at system's temporary directory (the output log will be deleted after all is done). Result, in lines per second, may vary depended on the randomizer engine work and use of gzip.
Useful to run on the clean installation, when we have no configs and templates yet.

Fixes: log rotation queue minimal length is 1 now, i.e. one log file + at least one rotation file, if needed.

## MANUAL

To install it, please read [Installation guide](inst.md)

logfilegen is controlled by the configuration variables and templates. The usual way to run logfilegen from the command line is, for example:


```console
logfilegen --template=mytemplate.tp --duration=20 --rate=100 --logfile=out.log
```

The **configuration variables** affects how logfilegen works, and **templates** affects the  generation of log file. There are many customization ways for logfilegen. To master them, you can read the following chapters:


[Using configuration](config.md)

[Using templates](templates.md)

Written by Peter Semiletov, 2023
