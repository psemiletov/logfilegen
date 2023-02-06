# logfilegen

Logfilegen is a fast and highly customizable tool to generate common server (nginx, etc) or user-defined format log files. You can generate log file with the desired rate (lines per second), the file size, lines count and the duration. Each variable of the log file can be redefined by the random or static value. logfilegen depends just on GCC or Clang with C++17 support, and, optionally, cmake.

## LINKS

[Github repo](https://github.com/psemiletov/logfilegen), [Latest release](https://github.com/psemiletov/logfilegen/releases/latest)

[Dockerhub repo](https://hub.docker.com/r/psemiletov/logfilegen/general)

[AUR](https://aur.archlinux.org/packages/logfilegen), [openSUSE](https://software.opensuse.org/package/logfilegen)


## NEWS

***06 february 2023, 1.1.0*** - The new release is slighty faster. Two new macros were added: @meta and @seq. All of them are described at the documentation, here in a short - @meta allows to combine several macros into one, so we can use more than one macro at the simple (non-sequenced) variable value. And @seq can be used to use sequences at macro, as in the usual variable value.

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
