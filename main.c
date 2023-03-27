#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "icmp_send.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf(stderr, "Invalid argument!");
        return EXIT_FAILURE;
    }

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    
    if (sockfd < 0) {
        printf(stderr, "%s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    struct sockaddr_in recipient;

    bzero(&recipient, sizeof(recipient));

    recipient.sin_family = AF_INET;
    
    if(inet_pton(AF_INET, argv[1], &recipient.sin_addr) == 0) {
        printf(stderr, "Invalid IP address!\n");
        return EXIT_FAILURE;
    }

    uint16_t id = (uint16_t)getpid();

    for(int i = 0; i < 30; i++) {
        send_packages(sockfd, id, i + 1, recipient);
    }
    return 0;
}