CC = gcc
CLINKS = -lm -lrt -pthread
CFLAGS = -Wall -g -o

OBJS1 = producer_or_consumer.o
TARGET1 = producer_or_consumer


compile: $(TARGET1) $(TARGET2)

$(TARGET1): $(OBJS1)
	$(CC) $^ $(CFLAGS) $@ $(CLINKS)


%.o: %.c
	$(CC) -c $^ $(CFLAGS) $@ $(CLINKS)

.PHONY: clean
clean:
	@rm -f *.o $(TARGET1) $(TARGET2)	
