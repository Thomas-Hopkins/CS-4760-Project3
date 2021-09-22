CC = gcc
CFLAGS = -Wall -g

LIBS = log

OUT = runsim

CLEAN = $(OUT) *.o

all: $(OUT)

$(OUT): $(OUT).o
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -f $(CLEAN)
