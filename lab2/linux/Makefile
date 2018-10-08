CC := cc
CFLAGS := -ansi -pedantic -g -I..
LIBS := -lrt -lpthread

all: producer

producer: producer.o togglecase.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

producer.o: producer.c ../common/togglecase.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm -rf $(wildcard *.o) producer
