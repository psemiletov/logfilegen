# logfilegen

The server log file generator

[![Release](../../actions/workflows/release.yml/badge.svg)](../../actions/workflows/release.yml)
[![Tests](../../actions/workflows/tests.yml/badge.svg)](../../actions/workflows/tests.yml)

[![Docker](../../actions/workflows/docker.yml/badge.svg)](../../actions/workflows/docker.yml)
[![Docker downloads](https://img.shields.io/docker/pulls/psemiletov/logfilegen.svg)](https://hub.docker.com/r/psemiletov/logfilegen)

[Dockerhub repo](https://hub.docker.com/r/psemiletov/logfilegen/general)

[AUR](https://aur.archlinux.org/packages/logfilegen)


THIS SOFTWARE IS IN ALPHA STATE!!!!

## PREFACE

All that is documented is work.


## INSTALLATION

To compile logfilegen from the source you need GCC/g++ or Clang with C++11 version support. If you did not compiled programs before, install g++ or Clang, and **make** utility to your system. Then, in the simplest case, run as root or with sudo, at the logfilegen source directory:


```console
make
make install
```

And to uninstall:


```console
make uninstall
```


