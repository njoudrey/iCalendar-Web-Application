CC = gcc
CFLAGS = -Wall -std=c11 -g

all: parser

parser: libcal.so

libcal.so: CalendarParser.o LinkedListAPI.o helpers.o
	$(CC) -shared -o ../libcal.so bin/CalendarParser.o bin/helpers.o bin/LinkedListAPI.o

LinkedListAPI.o: src/LinkedListAPI.c include/LinkedListAPI.h
	$(CC) $(CFLAGS) -c -fpic src/LinkedListAPI.c -Iinclude/ -o bin/LinkedListAPI.o

CalendarParser.o: src/CalendarParser.c include/CalendarParser.h
	$(CC) $(CFLAGS) -c -fpic src/CalendarParser.c -Iinclude/ -o bin/CalendarParser.o

helpers.o: src/helpers.c include/helpers.h
	$(CC) $(CFLAGS) -c -fpic src/helpers.c -Iinclude/ -o bin/helpers.o

clean:
	rm -rf bin/*.o bin/*.so main main.dSYM
