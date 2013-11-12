
GCC=g++

CFLAGS=-g -ggdb3

LIBS=-lpthread -lrt

INCLUDES=-I.

all: threading.o threading_test.o
	$(GCC) $(CFLAGS) threading.o threading_test.o $(LIBS)

threading_test.o: threading_test.cpp
	$(GCC) -c $(CFLAGS) $(INCLUDES) threading_test.cpp -o threading_test.o
    
threading.o: threading_linux.cpp threading.h
	$(GCC) -c $(CFLAGS) $(INCLUDES) threading_linux.cpp -o threading.o

clean:
	rm -f *.o a.out
