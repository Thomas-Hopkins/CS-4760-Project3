CC = gcc
CFLAGS = -Wall -g

LIBS = log

EXE = runsim testsim
DEPS = license.h config.h
OBJS = license.o

CLEAN = $(EXE) *.o $(OBJS)

all: $(EXE)

%sim: %sim.o $(OBJS) $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(OBJS) -Llib -l$(LIBS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -f $(CLEAN)
