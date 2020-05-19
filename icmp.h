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
#include <netdb.h>

#define INITIAL_SEQ_ID 1
#define PING_DELAY 1
#define NO_COMM_FLAGS 0
#define NS_IN_MS 1000000
#define MS_IN_S 1000

struct ping_params {
    char *address;
    int time_to_live;
};

struct echo_status {
    bool sent;
    bool received;
};

struct echo_reply {
    struct ip ip_header;
    struct icmphdr icmp_header;
};

struct echo_status icmp_send_echo(int skt, struct sockaddr_in *ping_address, int seq_id);
void icmp_skt_addr_init(const char *address, struct sockaddr_in *ping_address);
void icmp_ping(struct ping_params params);
_Noreturn void icmp_echo_loop(struct ping_params params);

#endif //PING_ICMP_H

