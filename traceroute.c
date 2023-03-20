#include "traceroute.h"

int traceroute(int socket_fd, struct sockaddr_in* socket) {
    int id = getpid();

    for(int i = 0; i < 30; i++) {
        struct in_addr reply[3];
        int r_time = 0;
        int i_reply = 0;
        send_packages(socket_fd, id, i + 1, *socket);
        int end = recall(socket_fd, id, i + i, &i_reply, &r_time, reply);
        error(end);
        traceroute_print(i, &i_reply, &r_time, reply);

        if(end) break;
    }
    return 0;
}

void traceroute_print(int ttl, int *i_reply, int *r_time, struct in_addr *reply) {
    if(ttl < 10) printf(" ");
    printf(" %d.\t", ttl);

    char ip1[20], ip2[20], ip3[20];
    inet_ntop(AF_INET, &(reply[0]), ip1, sizeof(ip1));
    inet_ntop(AF_INET, &(reply[1]), ip2, sizeof(ip2));
    inet_ntop(AF_INET, &(reply[2]), ip3, sizeof(ip3));

    if (*i_reply == 0) printf("*");
    else {
        printf("%s", ip1);
        if(strcmp(ip1, ip2) != 0 && *i_reply > 1) {
            printf(" %s", ip2);
        }
        if(strcmp(ip1, ip3) != 0 && strcmp(ip2, ip3) != 0 && *i_reply > 2) {
            printf(" %s", ip3);
        }
        if(*r_time < 0) printf("\t???");
        else printf("\tdms", *r_time);
    }
    printf("\n");
}