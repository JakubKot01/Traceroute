#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "traceroute.h"

u_int16_t compute_icmp_checksum (const void *buff, int length)
{
	u_int32_t sum;
	const u_int16_t* ptr = buff;
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (u_int16_t)(~(sum + (sum >> 16)));
}

int send_package(int sockfd, int pid, int ttl, struct sockaddr_in recipient) {
    struct icmphdr header;
    header.type = ICMP_ECHO;
    header.code = 0;
    header.un.echo.id = pid;
    header.un.echo.sequence = ttl;
    header.checksum = 0;
    header.checksum = compute_icmp_checksum((u_int16_t*)&header, sizeof(header));

    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));

    ssize_t bytes_sent = sendto (
        sockfd, 
        &header, 
        sizeof(header), 
        0, 
        (struct sockaddr*)&recipient, 
        sizeof(recipient)
        );
    if (bytes_sent < 0) {
        printf(stderr, "Sendto error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
}

int send_packages(int socket_fd, int pid, int ttl, struct sockaddr_in socket) {
    int i = 0;
    while(i < 3) {
        if(send_package(socket_fd, pid, ttl, socket) < 0)
            return EXIT_FAILURE;
        i++;
    }
    return EXIT_SUCCESS;
}