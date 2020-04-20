#include <stdio.h>
#include <unistd.h>
#include "icmp.h"

int main(int argc, char **argv) {
    char *address = argv[1];

    if (getuid() == 0) {
        // If TTL arg provided
        if (argc >= 3) {
            int time_to_live = atoi(argv[2]);
            icmp_ping(address, &time_to_live);
        } else {
            icmp_ping(address, NULL);
        }
    } else {
        perror("Ping requires sudo!\n");
    }
}
