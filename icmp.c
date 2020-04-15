#include "icmp.h"

void icmp_ping(char *address) {
    icmp_convert_to_ip(address);
    icmp_echo_loop(address);
}

_Noreturn void icmp_echo_loop(char *address) {
    while (1) {
        icmp_send_echo(address);
        icmp_time_delay(3);
    }
}

void icmp_send_echo(char *address) {
    printf("Pinging %s\n", address);
    // TODO send ICMP echo request
    // TODO calculate packet loss
    // TODO calculate RTT
}

void icmp_convert_to_ip(char *address) {
    printf("converting to ip\n");
    // TODO Convert Hostnames to IP addresses
}

void icmp_time_delay(int seconds) {
    time_t stop_time = time(0) + seconds;
    while (time(0) < stop_time);
}
