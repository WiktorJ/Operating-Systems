CC = gcc
CLINKS = -lpthread
CFLAGS = -Wall -g -o

OBJS1 = readers_writers.o
TARGET1 = readers_writers


compile: $(TARGET1) $(TARGET2)

$(TARGET1): $(OBJS1)
	$(CC) $^ $(CFLAGS) $@ $(CLINKS)


%.o: %.c
	$(CC) -c $^ $(CFLAGS) $@ $(CLINKS)

.PHONY: clean
clean:
	@rm -f *.o $(TARGET1) $(TARGET2)
