iCC = gcc
CLINKS = -lpthread
CFLAGS = -Wall -g -o

OBJS1 = record_counter.o
TARGET1 = record_counter


compile: $(TARGET1) $(TARGET2)

$(TARGET1): $(OBJS1)
	$(CC) $^ $(CFLAGS) $@ $(CLINKS)


%.o: %.c
	$(CC) -c $^ $(CFLAGS) $@ $(CLINKS)

.PHONY: clean
clean:
	@rm -f *.o $(TARGET1) $(TARGET2)
