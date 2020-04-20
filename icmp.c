#include "icmp.h"

void icmp_ping(char *address) {
    icmp_convert_to_ip(address);
    icmp_echo_loop(address);
}

_Noreturn void icmp_echo_loop(char *address) {
    int skt = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    int seq_id = 1;
    int packets_sent = 0, packets_received = 0;

    // TODO factor this out
    struct sockaddr_in ping_address;
    ping_address.sin_addr.s_addr = inet_addr(address);
    ping_address.sin_port = htons(0);
    ping_address.sin_family = AF_INET;

    struct echo_status current_status;

    while (1) {
        current_status = icmp_send_echo(skt, &ping_address, seq_id);

        if (current_status.sent) {
            packets_sent++;
        }

        if (current_status.received) {
            packets_received++;
        }

        if (packets_sent > 0) {
            printf("packets sent: %d\n", packets_sent);
            printf("packets received: %d\n", packets_received);
            printf("packet loss: %f\n", (double) (packets_sent - packets_received) * 100 / packets_sent);
        }

        icmp_time_delay(3);
        seq_id++;
    }
}

struct echo_status icmp_send_echo(int skt, struct sockaddr_in *ping_address, int seq) {
    bool sent = false, received = false;
    struct icmphdr packet;
    icmp_fill_packet(&packet, seq);
    struct sockaddr_in return_address;


    if (sendto(skt, &packet, sizeof(packet), 0, (const struct sockaddr *) ping_address, sizeof(*ping_address)) <= 0) {
        printf("sending failed\n");
    } else {
        printf("sending successful\n");
        sent = true;
    }

    int return_address_len = sizeof(return_address);
    if (recvfrom(skt, &packet, sizeof(packet), 0, (struct sockaddr*) &return_address, (socklen_t *) &return_address_len) <= 0) {
        printf("receiving failed\n");
    } else {
        printf("receiving successful\n");
        received = true;
    }

    return (struct echo_status){sent, received};
}

void icmp_fill_packet(struct icmphdr *packet, int seq) {
    // ICMP Echo Request
    packet->type = ICMP_ECHO;
    packet->code = 0;

    // Ping Process ID
    packet->un.echo.id = htons(getpid());

    // Increasing Sequence ID
    packet->un.echo.sequence = htons(seq);

    // IPv4 Checksum
    packet->checksum = 0;
    packet->checksum = ip_checksum(packet, sizeof(*packet));
}

void icmp_convert_to_ip(char *address) {
    printf("converting to ip\n");
    // TODO Convert Hostnames to IP addresses
}

void icmp_time_delay(int seconds) {
    time_t stop_time = time(0) + seconds;
    while (time(0) < stop_time);
}