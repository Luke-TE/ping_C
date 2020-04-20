#ifndef PING_ICMP_H
#define PING_ICMP_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "lib/checksum.h"
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define INITIAL_SEQ_ID 1
#define PING_DELAY 1
#define NO_COMM_FLAGS 0

struct echo_status {
    bool sent;
    bool received;
};

struct echo_reply {
    struct ip ip_layer;
    struct icmphdr icmp_layer;
};

struct echo_status icmp_send_echo(int skt, struct sockaddr_in *ping_address, int seq);
void icmp_skt_addr_init(const char *address, struct sockaddr_in *ping_address);
void icmp_ping(char *address);

_Noreturn void icmp_echo_loop(char *address);

#endif //PING_ICMP_H

