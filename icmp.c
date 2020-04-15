#include "icmp.h"

void icmp_convert_to_ip(char *address) {
    printf("converting to ip\n");
}

void icmp_send_echo(char *address) {
    printf("sending echo\n");
}

void icmp_time_delay(int seconds) {
    printf("time delay\n");
    time_t stop_time = time(0) + seconds;
    while (time(0) < stop_time);
}
