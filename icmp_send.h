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
#include <assert.h>

u_int16_t compute_icmp_checksum (const void *buff, int length);

int send_package(struct sockaddr_in recipient, int sockfd, int pid, int ttl, int seq);

int send_packages(int socket_fd, int pid, int ttl, struct sockaddr_in socket);