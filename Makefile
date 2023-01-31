ifndef prefix
prefix=/usr
endif

CXX = g++
#CXXFLAGS = -Wall -g -O3 -fopenmp
CXXFLAGS = -Wall -g -O3 -std=c++17


all: main.cpp
	$(CXX) $(CXXFLAGS) -o logfilegen utl.cpp params.cpp macro.cpp vars.cpp tpl.cpp pairfile.cpp cycle.cpp logrot.cpp main.cpp

clean:
	$(RM) logfilegen

uninstall:
	rm $(DESTDIR)$(prefix)/bin/logfilegen

install: logfilegen
	install -m 0755 logfilegen $(DESTDIR)$(prefix)/bin/logfilegen

test:
	logfilegen --help

#.PHONY: install
