CC = gcc
CFLAGS = -Wall -g

LIBS = log

EXE = runsim testsim
OBJS = license.o license.h config.h

CLEAN = $(EXE) *.o $(OBJS)

all: $(EXE)

%sim: %sim.o $(OBJS) 
	$(CC) $(CFLAGS) -o $@ $< -Llib -l$(LIBS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -f $(CLEAN)
