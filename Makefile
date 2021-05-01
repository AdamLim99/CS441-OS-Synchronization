#
# Samantha Foley
# CS 441/541: Project 5
#
#####################################################################
#
# Type "make" to compile your code
# 
# Type "make clean" to remove the executable (and any object files)
#
#####################################################################

CC=gcc
CFLAGS=-Wall -g -O0 -Ilib
LDFLAGS=-pthread

PROGS=costume

#
# List all of the binary programs you want to build here
# Separate each program with a single space
#
all: $(PROGS)

costume: costume.c costume.h lib/semaphore_support.o
	$(CC) -o costume  costume.c lib/semaphore_support.o $(CFLAGS) $(LDFLAGS)

#
# Cleanup the files that we have created
#
clean:
	$(RM) $(PROGS) *.o
	$(RM) -rf *.dSYM
	cd lib && make clean
