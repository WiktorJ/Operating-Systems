CC = gcc
CLINKS = -lrt
CFLAGS = -Wall -g -o

OBJS1 = client.o
OBJS2 = server.o
TARGET1 = client
TARGET2 = server

compile: $(TARGET1) $(TARGET2)

$(TARGET1): $(OBJS1)
	$(CC) $^ $(CFLAGS) $@ $(CLINKS)


$(TARGET2): $(OBJS2)
	$(CC) $^ $(CFLAGS) $@ $(CLINKS)


%.o: %.c
	$(CC) -c $^ $(CFLAGS) $@ $(CLINKS)

.PHONY: clean
clean:
	@rm -f *.o $(TARGET1) $(TARGET2)
