CC=g++
CFLAGS=-Wall -fPIC -g
DFLAGS=
LD_FLAGS= -lpthread
INCLUDES=

#LD_FLAGS=-L$(ACE_ROOT)/lib -lpthread -lACE
#INCLUDES=-I$(ACE_ROOT)

SOURCES=$(wildcard *.cpp)
TARGETS=$(patsubst %.cpp, %,$(SOURCES))
quiet-command = $(if $(v),$1,$(if $(2),@echo $2 && $1, @$1))

.PHONY:all clean
all:$(TARGETS)

%: %.cpp
	$(CC) $(INCLUDES) $(CFLAGS) $(DFLAGS) -o $@ $@.cpp $(LD_FLAGS)

clean:
	@rm -f $(TARGETS)
