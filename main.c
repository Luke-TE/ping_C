#include <stdio.h>
#include "icmp.h"

int main(int argc, char **argv) {
    char *address = argv[1];

    icmp_convert_to_ip(address);

    while (1) {
        icmp_send_echo(address);
        icmp_time_delay(3);
    }
}
