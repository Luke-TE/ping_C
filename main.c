#include <stdio.h>
#include "icmp.h"

int main(int argc, char **argv) {
    char *address = argv[1];
    icmp_ping(address);
}
