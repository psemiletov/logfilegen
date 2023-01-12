ifndef prefix
prefix=/usr
endif

CXX = g++
#CXXFLAGS = -Wall -g -O3 -fopenmp
CXXFLAGS = -Wall -g -O3


all: main.cpp
	$(CXX) $(CXXFLAGS) -std=c++11 -o logfilegen utl.cpp tpl.cpp pairfile.cpp main.cpp

clean:
	$(RM) logfilegen

uninstall:
	rm $(prefix)/bin/logfilegen

install: logfilegen
	install -m 0755 logfilegen $(prefix)/bin/logfilegen

test:
	logfilegen --help

#.PHONY: install
