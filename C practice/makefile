C = gcc
CFLAGS = -Wall -Werror
OUTPUT = args
DEBUG = -g
SOURCES = args.c


all:
	$(CC) $(SOURCES) $(CFLAGS) -o $(OUTPUT)
debug:
	$(CC) $(DEBUG) $(SOURCES) $(CFLAGS) -o $(OUTPUT)
clean:
	rm -f $(OUTPUT)
fresh:
	make clean
	make all
