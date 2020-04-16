#include "icmp.h"

void icmp_ping(char *address) {
    icmp_convert_to_ip(address);
    icmp_echo_loop(address);
}

_Noreturn void icmp_echo_loop(char *address) {
    int skt = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    // TODO factor this out
    struct sockaddr_in ping_address;
    ping_address.sin_addr.s_addr = inet_addr(address);
    ping_address.sin_port = htons(0); // TODO rewrite
    ping_address.sin_family = AF_INET;

    while (1) {
        icmp_send_echo(skt, &ping_address);
        icmp_time_delay(3);
    }

    // TODO calculate packet loss
}

void icmp_send_echo(int skt, struct sockaddr_in *ping_address) {
    struct icmphdr packet;
    struct sockaddr_in *return_address;
    icmp_fill_packet(&packet);
    
    if (sendto(skt, &packet, sizeof(packet), 0, (const struct sockaddr *) ping_address, sizeof(*ping_address)) <= 0) {
        printf("sending failed\n");
    } else {
        printf("sending successful\n");
    }
    if ( recvfrom(skt, &packet, sizeof(packet), 0, (struct sockaddr *) return_address, sizeof(*return_address)) <= 0) {
        printf("receiving failed\n");
    } else {
        printf("receiving successful\n");
    }

    // TODO calculate RTT
}

void icmp_fill_packet(struct icmphdr *packet) {
    packet->type = ICMP_ECHO;
    packet->code = 0;
    packet->checksum = 0; // TODO proper checksum
    packet->un.echo.id = 12345;
    packet->un.echo.sequence = 0;
}

void icmp_convert_to_ip(char *address) {
    printf("converting to ip\n");
    // TODO Convert Hostnames to IP addresses
}

void icmp_time_delay(int seconds) {
    time_t stop_time = time(0) + seconds;
    while (time(0) < stop_time);
}
