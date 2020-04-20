#ifndef PING_ICMP_H
#define PING_ICMP_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "lib/checksum.h"
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

struct echo_status {
    bool sent;
    bool received;
};

void icmp_convert_to_ip(char *address);
struct echo_status icmp_send_echo(int skt, struct sockaddr_in *ping_address, int seq);
void icmp_time_delay(int seconds);
void icmp_ping(char *address);
void icmp_fill_packet(struct icmphdr *packet, int seq);

_Noreturn void icmp_echo_loop(char *address);

#endif //PING_ICMP_H

