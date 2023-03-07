## Installation

To compile logfilegen from the source you need GCC/g++ or Clang with C++17 version support. If you did not compiled programs before, install GCC/g++ or Clang, **cmake** and **make** utility to your system.

Use the tarball with the [latest release](https://github.com/psemiletov/logfilegen/releases/latest), it is always stable. Altough logfilegen has several manually written makefiles, the prefered way to compile logfilegen is CMake/make pair.


### Build the source with CMake/Make

Unpack the source, go to the logfilegen source dir, and, as root or with sudo, run:


#### Default, build with GCC


```
mkdir b
cd b
cmake ..
make
make install
```

#### Default, build with Clang++


```
mkdir b
cd b
cmake -DUSE_CLANG ..
make
make install
```

#### Static build


```
mkdir b
cd b
cmake -DUSE_STATIC=ON ..
make
make install
```


#### With prometheus_cpp support

logfilegen has support for Prometheus/OpenMetrics format metrics exposion via built-in server code or, as an optional and experimatal feature, via [prometheus-cpp](https://github.com/jupp0r/prometheus-cpp) library. Currently, the built-in server code works faster. But if you want to play around with prometheus_cpp:


```
mkdir b
cd b
cmake -DUSE_PROMCPP=ON ..
make
make install
```



### From the source with plain Make and default compiler (usually GCC)


More simple, but less powerful and less flexlible way to compile logfilegen, is the use on manual written Makefiles:


```console
make
make install
```

And to uninstall:

```console
make uninstall
```

### From the source with plain Make and Clang

```console
make CXX=clang++
```

There are some ready-to-use examples below to build logfilegen with Makefiles on some systems or distros:



#### Ubuntu 22.04

- Download and unpack the tarball. Go to the unpacked source directory.

- Install dependencies (do ```sudo apt update``` if didn't before):

```console
sudo apt install make g++
```

- Build and install:

```console
make
sudo make install
```


#### Amazon Linux 2

- Download and unpack the tarball. Go to the unpacked source directory.

- Install dependencies:

```console
sudo yum install git make gcc-c++
```

- Build and install:

```console
make
sudo make install
```


#### OpenBSD

- Download and unpack the tarball. Go to the unpacked source directory.

- Install dependencies:

```console
pkg_add llvm
pkg_add make
```
- Build and install:


```console
make --makefile=Makefile.OpenBSD
make install
```



### READ MORE

[Configuration](https://psemiletov.github.io/logfilegen/config.html)

[Using templates](https://psemiletov.github.io/logfilegen/templates.html)

[Using metrics](metrics.md)
