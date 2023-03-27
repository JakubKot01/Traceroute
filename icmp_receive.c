#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "icmp_send.h"
#include "icmp_receive.h"

void print_as_bytes (unsigned char* buff, ssize_t length)
{
	for (ssize_t i = 0; i < length; i++, buff++)
		printf ("%.2x ", *buff);	
}


int receive(int sockfd, int ttl, int id) {
	
	fd_set descriptors;
	FD_ZERO (&descriptors);
	FD_SET(sockfd, &descriptors);
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;

    int result;

    int i = 0;
	while(i < 3) {
		FD_ZERO (&descriptors);
		FD_SET(sockfd, &descriptors);

		int ready = select(sockfd+1, &descriptors, NULL, NULL, &tv);

		if(ready < 0) return -1;
		if(ready == 0) break;

		struct sockaddr_in 	sender;	
		socklen_t 			sender_len = sizeof(sender);
		u_int8_t 			buffer[IP_MAXPACKET];

		ssize_t packet_len = recvfrom (sockfd, buffer, IP_MAXPACKET, 0, (struct sockaddr*)&sender, &sender_len);
	
		if (packet_len < 0) {
			fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
			return EXIT_FAILURE;
		}

		char sender_ip_str[20]; 

		inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
		printf ("Received IP packet with ICMP content from: %s\n", sender_ip_str);

		struct ip* 			ip_header = (struct ip*) buffer;
		ssize_t				ip_header_len = 4 * ip_header->ip_hl;

		struct icmp *header = (struct icmp *)((uint8_t *)buffer + ip_header_len);

        if (header->icmp_type == ICMP_ECHOREPLY) {
            if(header->icmp_seq == ttl + i && header->icmp_id == id) {
                result = EXIT_SUCCESS;
                i++;
            }
        }
        if (header->icmp_type == ICMP_TIME_EXCEEDED) {

            struct ip*          temp_ip_header = (struct ip*)((uint8_t *)header + 8);
            ssize_t             temp_ip_header_len = 4 * temp_ip_header->ip_hl;
            
            struct icmp *temp_header = (struct icmp*)
                                ((uint8_t *)temp_ip_header + temp_ip_header_len * 4);

            if (temp_header->icmp_seq == ttl + i && temp_header->icmp_id == id) i++;
        }
	}


	return EXIT_SUCCESS;
}
