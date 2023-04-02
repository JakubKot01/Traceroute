/*
    Jakub Kot 324067
*/

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

void print_as_bytes (unsigned char* buff, ssize_t length);

int receive(int sockfd, int ttl, int id, struct timeval send_time);

void printer(int counter, int ttl, char* senders[3], uint64_t avg);