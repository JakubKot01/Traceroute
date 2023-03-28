#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <time.h>

#include "icmp_send.h"
#include "icmp_receive.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Invalid argument!");
        return EXIT_FAILURE;
    }

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    
    if (sockfd < 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    struct sockaddr_in recipient;

    bzero(&recipient, sizeof(recipient));

    recipient.sin_family = AF_INET;
    
    if(inet_pton(AF_INET, argv[1], &recipient.sin_addr) == 0) {
        fprintf(stderr, "Invalid IP address!\n");
        return EXIT_FAILURE;
    }

    uint16_t id = (uint16_t)getpid();

    struct timeval curr_time;

    for(int i = 0; i < 30; i++) {
        gettimeofday(&curr_time, NULL);
        send_packages(sockfd, id, i + 1, recipient);
        int rec = receive(sockfd, i + 1, id, curr_time);
        if(rec < 0) {
            fprintf(stderr, "Cannot receive packets %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        if(rec == 1) {
            break;
        }
    }
    close(sockfd);
    return 0;
}