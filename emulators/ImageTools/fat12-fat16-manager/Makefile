CXXTARGET=
CTARGET=fat

CXX=g++
CC=gcc

CXXFLAGS=-c -Wall -std=c++11
CFLAGS=-c -Wall
LDFLAGS=-Wall

HEADERS=$(wildcard *.h) $(wildcard */*.h)
CXXSOURCES=$(wildcard *.cxx) $(wildcard */*.cxx)
CXXOBJECTS=$(patsubst %.cxx, %.o, $(CXXSOURCES))
CSOURCES=$(wildcard *.c) $(wildcard */*.c)
COBJECTS=$(patsubst %.c, %.o, $(CSOURCES))

.PHONY: cxxbuild cbuild all default clean
.PRECIOUS: $(CXXTARGET) $(CTARGET) $(CXXOBJECTS) $(COBJECTS)

all: clean default
default: cbuild
cxxbuild: $(CXXTARGET)
	@echo "Build cxx-project"
cbuild: $(CTARGET)
	@echo "Build c-project"

$(CXXTARGET): $(CXXOBJECTS)
	@echo "(CXX) $?"
	@$(CXX) $(CXXOBJECTS) $(LDFLAGS) -o $@

$(CTARGET): $(COBJECTS)
	@echo "(CC) $?"
	@$(CC) $(COBJECTS) $(LDFLAGS) -o $@
	
%.o: %.cxx $(HEADERS)
	@echo "(CXX) $<"
	@$(CXX) $(CXXFLAGS) $< -o $@

%.o: %.c $(HEADERS)
	@echo "(CC) $<"
	@$(CC) $(CFLAGS) $< -o $@

clean:
	@echo "Clean project"
	@rm -rfv *.o */*.o $(CXXTARGET) $(CTARGET)
