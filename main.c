#include <stdio.h>
#include <unistd.h>
#include "icmp.h"

int main(int argc, char **argv) {
    char *address = argv[1];

    if (getuid() == 0) {
        icmp_ping(address);
    } else {
        perror("Ping requires sudo!\n");
    }
}
