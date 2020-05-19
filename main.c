#include <stdio.h>
#include <unistd.h>
#include "icmp.h"

#define ERROR -1

int main(int argc, char **argv) {
    if (argc < 2) {
        perror("Missing address argument.");
        exit(ERROR);
    }

    struct ping_params params;
    params.address = argv[1];
    params.time_to_live = 0;

    if (argc >= 3) {
        params.time_to_live = atoi(argv[2]);
    }

    // Only root user can echo
    if (getuid() == 0) {
        icmp_ping(params);
    } else {
        perror("Ping requires sudo!\n");
    }
}
