#ifndef UNTITLED_ICMP_H
#define UNTITLED_ICMP_H

#include <stdio.h>
#include <time.h>

void icmp_convert_to_ip(char *address);
void icmp_send_echo(char *address);
void icmp_time_delay(int seconds);

#endif //UNTITLED_ICMP_H
