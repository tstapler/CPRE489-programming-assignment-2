CC=gcc
CC_FLAGS=-std=c11 -g -ggdb -gdwarf-3

all: sender receiver

.DEFAULT_GOAL:=all

sender: sender.o arq_packet.o
	$(CC) $(CC_FLAGS) ccitt16.o arq_packet.o sender.o -o sender

receiver: receiver.o arq_packet.o
	$(CC) $(CC_FLAGS) ccitt16.o arq_packet.o receiver.o -o receiver

.o: 
	$(CC) -c $@
