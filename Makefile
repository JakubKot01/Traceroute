CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra

main: Makefile main.o send.o receive.o $(CC) $(CFLAGS) -o main main.o send.o receive.o

main.o: Makefile main.c
		$(CC) $(CFLAGS) -c main.c -o main.o

send.o: Makefile icmp_send.c icmp_send.h
		$(CC) $(CFLAGS) -c icmp_send.c -o send.o

receive.o : Makefile icmp_receive.c icmp_receive.h 
		$(CC) $(CFLAGS) -c icmp_receive.c -o receive.o