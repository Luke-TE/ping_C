#include "icmp.h"

void handle_dest_unreachable(int code);

void handle_time_exceeded(int code);

char *try_convert_to_ip(char *hostname);

void set_echo_hdr(struct icmphdr *packet, int seq);

void time_delay(int seconds);

bool handle_reception(struct echo_reply *echo_reply_packet);


void print_rtt(struct timespec rtt_start, struct timespec rtt_end);

void icmp_ping(struct ping_params params) {
    params.address = try_convert_to_ip(params.address);
    icmp_echo_loop(params);
}

_Noreturn void icmp_echo_loop(struct ping_params params) {
    int seq_id = INITIAL_SEQ_ID;
    int packets_sent = 0, packets_received = 0;
    double packet_loss;

    // Initialize socket
    int skt = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    // Set TTL, if provided
    if (params.time_to_live != 0) {
        printf("Time To Live set to %d. \n", params.time_to_live);
        setsockopt(skt, IPPROTO_IP, IP_TTL, &params.time_to_live, sizeof(int));
    }

    // Initialize address to ping
    struct sockaddr_in ping_address;
    icmp_skt_addr_init(params.address, &ping_address);

    struct echo_status current_status;

    // Endless echo loop
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
        printf("Packet loss: %.2f%%, ", packet_loss);
        printf("\n\n");

        time_delay(PING_DELAY);
        seq_id++;
    }
}

struct echo_status icmp_send_echo(int skt, struct sockaddr_in *ping_address, int seq_id) {
    bool sent = false, received = false;
    struct timespec rtt_start, rtt_end;

    // Set the ICMP request's header to be an echo request
    struct icmphdr echo_req_packet;
    set_echo_hdr(&echo_req_packet, seq_id);

    struct echo_reply echo_reply_packet;


    printf("Sending request... ");
    clock_gettime(CLOCK_MONOTONIC_RAW, &rtt_start);
    if (sendto(skt, &echo_req_packet, sizeof(echo_req_packet), NO_COMM_FLAGS, (const struct sockaddr *) ping_address,
               sizeof(*ping_address)) <= 0) {
        printf("Sending request failed. \n");
        return (struct echo_status) {sent, received};
    } else {
        printf("Sending request successful. \n");
        sent = true;
    }

    // Setting up return address variable for reception
    struct sockaddr_in received_address;
    int return_address_len = sizeof(received_address);


    printf("Receiving request... ");
    ssize_t num_bytes_recv = recvfrom(skt, &echo_reply_packet, sizeof(echo_reply_packet), NO_COMM_FLAGS,
                                      (struct sockaddr *) &received_address, (socklen_t *) &return_address_len);
    clock_gettime(CLOCK_MONOTONIC_RAW, &rtt_end);
    if (num_bytes_recv <= 0) {
        printf("Receiving reply failed. \n");
    } else {
        received = handle_reception(&echo_reply_packet);
        printf("\n");

        // Get actual IP address communicated with
        char *address_pinged = inet_ntoa((struct in_addr) {received_address.sin_addr.s_addr});
        printf("Return address was %s:%d\n", address_pinged, received_address.sin_port);
    }

    print_rtt(rtt_start, rtt_end);
    printf("\n");
    return (struct echo_status) {sent, received};
}

void print_rtt(struct timespec rtt_start, struct timespec rtt_end) {
    // Calculate and display round trip time
    long double rtt = (double) ((rtt_end).tv_sec - (rtt_start).tv_sec) * MS_IN_S +
                      (double) ((rtt_end).tv_nsec - (rtt_start).tv_nsec) / NS_IN_MS;
    printf("Round-trip time %.2Lf ms \n", rtt);
}

bool handle_reception(struct echo_reply *echo_reply_packet) {
    // Reception of echo successful, interpret the response type
    switch (echo_reply_packet->icmp_header.type) {
        case ICMP_ECHOREPLY:
            printf("Receiving reply successful. \n");
            return true;
        case ICMP_DEST_UNREACH:
            printf("Destination unreachable. ");
            handle_dest_unreachable((*echo_reply_packet).icmp_header.code);
            break;
        case ICMP_TIME_EXCEEDED:
            printf("Time exceeded. ");
            handle_time_exceeded((*echo_reply_packet).icmp_header.code);
            break;
        default:
            printf("Error Message with type %d, code %d. \n", (*echo_reply_packet).icmp_header.type,
                   (*echo_reply_packet).icmp_header.code);
    }
    return false;
}

char *try_convert_to_ip(char *hostname) {
    // Try to convert hostname tp IP address if possible
    struct hostent *host = gethostbyname(hostname);
    struct in_addr **addr_list = (struct in_addr **) host->h_addr_list;

    if (addr_list[0] != NULL) {
        printf("Converted to IP address. \n");
        return inet_ntoa(*addr_list[0]);
    }

    // Unable to convert to IP address
    return hostname;
}

void icmp_skt_addr_init(const char *address, struct sockaddr_in *ping_address) {
    // Initialize IPv4 address
    ping_address->sin_addr.s_addr = inet_addr(address);
    ping_address->sin_port = htons(0);
    ping_address->sin_family = AF_INET;
}

void set_echo_hdr(struct icmphdr *packet, int seq) {
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

void time_delay(int seconds) {
    time_t stop_time = time(0) + seconds;

    // Busy wait until time has passed
    while (time(0) < stop_time);
}

void handle_dest_unreachable(int code) {
    // Print type of destination unreachable error
    int dest_unreachable_errors = 16;
    char *error_messages[] = {"Network was unreachable.",
                              "Host was unreachable.",
                              "Protocol was unreachable.",
                              "Port was unreachable.",
                              "Fragmentation required.",
                              "Source route failed.",
                              "Unknown destination network.",
                              "Unknown destination host.",
                              "Source host isolated.",
                              "Network prohibited by administrator.",
                              "Host prohibited by administrator.",
                              "Network was unreachable for ToS.",
                              "Host was unreachable for ToS.",
                              "Communication prohibited by administrator.",
                              "Host precedence violation.",
                              "Precedence cutoff in effect."};

    if (code >= 0 && code < dest_unreachable_errors) {
        printf("%s\n", error_messages[code]);
    } else {
        printf("Unknown code %d occurred.\n", code);
    }

}

void handle_time_exceeded(int code) {
    // Print type of time exceeded error
    switch (code) {
        case ICMP_EXC_TTL:
            printf("TTL count exceeded.\n");
            break;
        case ICMP_EXC_FRAGTIME:
            printf("Fragment Reassembly time exceeded.\n");
            break;
        default:
            printf("Unknown code %d occurred.\n", code);
    }
}