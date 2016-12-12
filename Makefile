CC=gcc
CC_FLAGS=-std=c11 -g -ggdb -gdwarf-3

all: sender receiver

.DEFAULT_GOAL:=all

sender: sender.o helper_functions.o AddCongestion.o
	$(CC) $(CC_FLAGS) ccitt16.o helper_functions.o sender.o -o sender

receiver: receiver.o helper_functions.o
	$(CC) $(CC_FLAGS) ccitt16.o helper_functions.o receiver.o -o receiver

.o: 
	$(CC) -c $@
