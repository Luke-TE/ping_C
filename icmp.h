#ifndef PING_ICMP_H
#define PING_ICMP_H

#include <stdio.h>
#include <time.h>

void icmp_convert_to_ip(char *address);
void icmp_send_echo(char *address);
void icmp_time_delay(int seconds);
void icmp_ping(char *address);

_Noreturn void icmp_echo_loop(char *address);

#endif //PING_ICMP_H
