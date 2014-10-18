#include "udp_load_balancer.h"


struct sockaddr_in get_server_address(char *ip, int port);

int create_socket(unsigned int port);

void listen_udp_packet(
    char message[MAX_PAYLOAD_LENGTH],
    unsigned int *message_length,
    int socket_
);

void send_udp_packet(
    char *ip,
    int port,
    char message[MAX_PAYLOAD_LENGTH],
    int message_length,
    int socket
);