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
#include <sys/time.h>

#include "icmp_receive.h"

void printer(int counter, int ttl, char* senders[3], uint64_t avg) {
    printf("%d. ", ttl);
    printf("%s ", senders[0]);
    if(counter > 1 && strcmp(senders[0], senders[1])) {
        printf("%s ", senders[1]);
    }
    if(counter > 2 && strcmp(senders[0], senders[2])) {
        printf("%s ", senders[2]);
    }
    printf("\t");

    if(counter == 0) {
        printf("*\n");
    }
    if(counter < 3) {
        printf("???\n");
    }
    else {
        printf("%ld ms\n", avg);
    }
}


int receive(int sockfd, int ttl, int id, struct timeval send_time) {
	
	fd_set descriptors;
    //Usuwamy wszystkie deskryptory z descriptors
	FD_ZERO (&descriptors);
    //Dodajemy sockfd do descriptors
	FD_SET(sockfd, &descriptors);
	struct timeval tv, curr_time;
	tv.tv_sec = 1;
	tv.tv_usec = 0;

    int result = 0;

    uint64_t diff_times[3];
    uint64_t millis[3];

    const char* senders[3];

    uint64_t send_millis = ((uint64_t)send_time.tv_sec * (uint64_t)1000) + ((uint64_t)send_time.tv_usec / 1000);

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

		ssize_t packet_len = recvfrom(sockfd, buffer, IP_MAXPACKET, 0, (struct sockaddr*)&sender, &sender_len);
	
		if (packet_len < 0) {
			fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}

		char sender_ip_str[20];

		inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));

        senders[i] = sender_ip_str;

		struct ip* 			ip_header = (struct ip*) buffer;
		ssize_t				ip_header_len = 4 * ip_header->ip_hl;

		struct icmp *header = (struct icmp *)((uint8_t *)buffer + ip_header_len);

        if (header->icmp_type == ICMP_ECHOREPLY) {

            if(header->icmp_seq == ttl * 3 + i && header->icmp_id == id) {
                gettimeofday(&curr_time, NULL);
                millis[i] = ((uint64_t)curr_time.tv_sec * (uint64_t)1000) + ((uint64_t)curr_time.tv_usec / 1000);
                diff_times[i] = millis[i] - send_millis;
                ++i;
                result = 1;
            }
        }
        if (header->icmp_type == ICMP_TIME_EXCEEDED) {

            struct ip*          temp_ip_header = (struct ip*)((uint8_t *)header + 8);
            ssize_t             temp_ip_header_len = 4 * temp_ip_header->ip_hl;

            struct icmp *temp_header = (struct icmp*)
                                ((uint8_t *)temp_ip_header + temp_ip_header_len);

            if (temp_header->icmp_seq == ttl * 3 + i && temp_header->icmp_id == id) {
                gettimeofday(&curr_time, NULL);
                millis[i] = ((uint64_t)curr_time.tv_sec * (uint64_t)1000) + ((uint64_t)curr_time.tv_usec / 1000);
                diff_times[i] = millis[i] - send_millis;
                ++i;
            }
        }
	}

    uint64_t avg = (diff_times[0] + diff_times[1] + diff_times[2]) / 3;
    printer(i, ttl, senders, avg);
	return result;
}
