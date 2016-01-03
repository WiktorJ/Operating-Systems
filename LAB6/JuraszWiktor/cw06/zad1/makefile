CC = gcc
CFLAGS = -Wall -g -I.
OBJS = server.o client.o

all: $(OBJS)

%.o : %.c chat.h
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) -o $* $@ 

.PHONY : clean
clean:
	@rm -f $(OBJS)

.PHONY : clean-all
clean-all:
	@rm -f $(OBJS) $(OBJS:.o=)
