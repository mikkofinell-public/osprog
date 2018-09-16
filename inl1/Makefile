CC := cc
CFLAGS := -ansi -pedantic -g
LIBS := -lrt -lpthread

all: consumer producer

consumer: consumer.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

consumer.o: consumer.c
	$(CC) $(CFLAGS) -c $<

producer: producer.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

producer.o: producer.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(wildcard *.o) producer consumer
