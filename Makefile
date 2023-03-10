ifndef prefix
prefix=/usr
endif




#CXXFLAGS = -Wall -g -O3 -fopenmp


CXXFLAGS = -Wall -g -O3 -std=c++11 -pthread -lpthread
#LDFLAGS= -pthread  -lpthread

#add for -lpthread

#OS_NAME := $(shell uname -s | tr A-Z a-z)

#os:
#	@echo $(OS_NAME)

# Detect operating system in Makefile.
# Author: He Tao
# Date: 2015-05-30

OSFLAG 				:=
ifeq ($(OS),Windows_NT)
#	OSFLAG += -D WIN32
	CXXFLAGS += -DWIN32

ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		OSFLAG += -D AMD64
	endif
	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		OSFLAG += -D IA32
	endif
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		OSFLAG += -D LINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		OSFLAG += -D OSX
	endif
		UNAME_P := $(shell uname -p)
	ifeq ($(UNAME_P),x86_64)
		OSFLAG += -D AMD64
	endif
		ifneq ($(filter %86,$(UNAME_P)),)
	OSFLAG += -D IA32
		endif
	ifneq ($(filter arm%,$(UNAME_P)),)
		OSFLAG += -D ARM
	endif
endif



# detect what shell is used
ifeq ($(findstring cmd.exe,$(SHELL)),cmd.exe)
$(info "shell Windows cmd.exe")
DEVNUL := NUL
WHICH := where
else
$(info "shell Bash")
DEVNUL := /dev/null
WHICH := which
endif



#ifndef CXX
# detect platform independently if gcc is installed
ifeq ($(shell ${WHICH} clang++ 2>${DEVNUL}),)
$(error "clang++ is not in your system PATH")
ifndef CXX
CXX = g++
endif
else
$(info "clang++ found")
ifndef CXX
CXX = clang++
endif
endif
#endif


ifeq ($(OS),Windows_NT)
all: main.cpp
	$(CXX) $(CXXFLAGS) -o logfilegen utl.cpp params.cpp macro.cpp vars.cpp tpl.cpp pairfile.cpp cycle.cpp logrot.cpp main.cpp -lws2_32
else
#	@echo $(OSFLAG)
all: main.cpp
	$(CXX) $(CXXFLAGS) -o logfilegen utl.cpp params.cpp macro.cpp vars.cpp tpl.cpp pairfile.cpp cycle.cpp logrot.cpp main.cpp

endif



clean:
	$(RM) logfilegen

uninstall:
	rm $(DESTDIR)$(prefix)/bin/logfilegen

install: logfilegen
	install -m 0755 logfilegen $(DESTDIR)$(prefix)/bin/logfilegen

test:
	logfilegen --help

#.PHONY: install
