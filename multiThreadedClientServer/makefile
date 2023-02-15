C = gcc
OUTPUT = client
OUTPUT2 = server
DEBUG = -g
SOURCES = client.c
SOURCES2 = server.c


all:
	$(CC) $(SOURCES) -o $(OUTPUT)
	$(CC) $(SOURCES2) -o $(OUTPUT2) -ljson-c
debug:
	$(CC) $(DEBUG) $(SOURCES) $(CFLAGS) -o $(OUTPUT)
	$(CC) $(DEBUG) $(SOURCES2) $(CFLAGS) -o $(OUTPUT2) -ljson-c
clean:
	rm -f $(OUTPUT)
	rm -f $(OUTPUT2)
fresh:
	make clean
	make all