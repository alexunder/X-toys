# makefile for vty
CC=gcc
LD=$(CC)

INC=-I"./include"
SRC=./src
APP=vty
OBJS=$(patsubst %.c,%.o,$(wildcard $(SRC)/*.c))

CFLAGS=$(INC)

all: $(APP)

$(APP): $(OBJS)
	$(LD) -o $@ $^

%.o:    %.c
	$(CC) -c -g $(CFLAGS) $< -o $@

clean:
	@rm -f $(SRC)/*.o
