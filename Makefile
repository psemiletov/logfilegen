prefix=/usr/local

all: main.cpp
	g++ -g -Wall -O3 -o logfilegen tpl.cpp libretta_pairfile.cpp main.cpp

clean:
	$(RM) logfilegen

uninstall:
	rm $(prefix)/bin/logfilegen

install: logfilegen
	install -m 0755 logfilegen $(prefix)/bin

.PHONY: install
