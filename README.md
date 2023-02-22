# logfilegen

The server log file generator

[![Release](../../actions/workflows/release_cmake.yml/badge.svg)](../../actions/workflows/release_cmake.yml)
[![Tests](../../actions/workflows/tests.yml/badge.svg)](../../actions/workflows/tests.yml)

[![Docker](../../actions/workflows/docker.yml/badge.svg)](../../actions/workflows/docker.yml)
[![Docker downloads](https://img.shields.io/docker/pulls/psemiletov/logfilegen.svg)](https://hub.docker.com/r/psemiletov/logfilegen)

[Official site](https://psemiletov.github.io/logfilegen/)

[Dockerhub repo](https://hub.docker.com/r/psemiletov/logfilegen/general)

[AUR](https://aur.archlinux.org/packages/logfilegen), [openSUSE](https://software.opensuse.org/package/logfilegen)


## ABOUT

Logfilegen is a fast and highly customizable tool to generate server (nginx, etc) or user-defined format log files. You can generate log file with the desired rate (lines per second), the file size, lines count and the duration. Each variable of the log file can be redefined by the random or static value. logfilegen depends just on GCC or Clang with C++17 support, and, optionally, cmake.

## USE

The server log file generator. logfilegen is controlled by the configuration variables and templates. The usual way to run logfilegen from the command line is, for example:


```console
logfilegen --template=mytemplate.tp --duration=20 --rate=100 --logfile=out.log
```

The **configuration variables** affects how logfilegen works, and **templates** affects the  generation of log file.

Please read the Manual:

[Installation](https://psemiletov.github.io/logfilegen/inst.html)

[Configuration](https://psemiletov.github.io/logfilegen/config.html)

[Using templates](https://psemiletov.github.io/logfilegen/templates.html)

[Using metrics](https://psemiletov.github.io/logfilegen/metrics.html)
