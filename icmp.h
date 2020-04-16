#ifndef PING_ICMP_H
#define PING_ICMP_H

#include <stdio.h>
#include <time.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void icmp_convert_to_ip(char *address);
void icmp_send_echo(int skt, struct sockaddr_in *ping_address);
void icmp_time_delay(int seconds);
void icmp_ping(char *address);
void icmp_fill_packet(struct icmphdr *packet);

_Noreturn void icmp_echo_loop(char *address);

#endif //PING_ICMP_H
