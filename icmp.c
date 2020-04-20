#include "icmp.h"

void icmp_ping(char *address) {
    icmp_convert_to_ip(address);
    icmp_echo_loop(address);
}

_Noreturn void icmp_echo_loop(char *address) {
    int seq_id = INITIAL_SEQ_ID;
    int packets_sent = 0, packets_received = 0;
    double packet_loss;

    // Initialize socket
    int skt = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    // Initialize address to ping
    struct sockaddr_in ping_address;
    icmp_skt_addr_init(address, &ping_address);

    struct echo_status current_status;

    while (true) {
        // Ping the address
        current_status = icmp_send_echo(skt, &ping_address, seq_id);

        if (current_status.sent) {
            packets_sent++;
        }

        if (current_status.received) {
            packets_received++;
        }

        // Calculate packet loss if possible
        if (packets_sent > 0) {
            packet_loss = (double) (packets_sent - packets_received) * 100 / packets_sent;
        }

        // Print details of echo request
        printf("Packets sent: %d, ", packets_sent);
        printf("Packets received: %d, ", packets_received);
        printf("Packet loss: %f, ", packet_loss);
        printf("\n\n");

        icmp_time_delay(PING_DELAY);
        seq_id++;
    }
}

struct echo_status icmp_send_echo(int skt, struct sockaddr_in *ping_address, int seq) {
    bool sent = false, received = false;

    // Set the ICMP request's header to be a echo request
    struct icmphdr echo_packet;
    icmp_set_echo_hdr(&echo_packet, seq);


    printf("Sending request... ");
    if (sendto(skt, &echo_packet, sizeof(echo_packet), NO_COMM_FLAGS, (const struct sockaddr *) ping_address,
               sizeof(*ping_address)) <= 0) {
        printf("Sending request failed. \n");
    } else {
        printf("Sending request successful. \n");
        sent = true;
    }

    // Setting up return address variable for reception
    struct sockaddr_in received_address;
    int return_address_len = sizeof(received_address);

    printf("Receiving request... ");
    if (recvfrom(skt, &echo_packet, sizeof(echo_packet), NO_COMM_FLAGS, (struct sockaddr *) &received_address,
                 (socklen_t *) &return_address_len) <= 0) {
        printf("Receiving reply failed. \n");
    } else {
        printf("Receiving reply successful. \n");

        printf("%d %d %d %d\n", echo_packet.type, echo_packet.code, echo_packet.un.echo.sequence, echo_packet.un.echo.id);

        char *address_pinged = inet_ntoa((struct in_addr) {received_address.sin_addr.s_addr});
        printf("Return address was %s:%d\n", address_pinged, received_address.sin_port);
        received = true;
    }


    printf("\n");

    return (struct echo_status) {sent, received};
}

void icmp_convert_to_ip(char *address) {
    printf("converting to ip\n");
    // TODO Convert Hostnames to IP addresses
}

void icmp_skt_addr_init(const char *address, struct sockaddr_in *ping_address) {
    ping_address->sin_addr.s_addr = inet_addr(address);

    // TODO Set the port to ping
    ping_address->sin_port = htons(0);

    // Initialize IPv4 address
    ping_address->sin_family = AF_INET;
}

void icmp_set_echo_hdr(struct icmphdr *packet, int seq) {
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

void icmp_time_delay(int seconds) {
    time_t stop_time = time(0) + seconds;

    // Busy wait until time has passed
    while (time(0) < stop_time);
}