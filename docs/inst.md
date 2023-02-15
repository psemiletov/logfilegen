## Installation

### From the source with plain Make

To compile logfilegen from the source you need GCC/g++ or Clang with C++17 version support. If you did not compiled programs before, install g++ or Clang, and **make** utility to your system.

First of all, download the tarball from [latest release](https://github.com/psemiletov/logfilegen/releases/latest), unpack it, and, if you are already familiar with compilation tools, go to the logfilegen source dir, and, as root or with sudo, run:

```console
make
make install
```

And to uninstall:

```console
make uninstall
```

There are some ready-to-use examples below to build logfilegen on some systems or distros:


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


### From the source with CMake/Make


#### Default


```
mkdir b
cd b
cmake ..
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



#### With prometheus_cpp support


```
mkdir b
cd b
cmake -DUSE_PROM=ON ..
make
make install
```
