#include <stdlib.h>
#include <arpa/inet.h>

#include "udp_load_balancer.h"


struct sockaddr_in get_server_address(char *ip, int port)
{
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    if(ip == NULL) {
        server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        server_address.sin_addr.s_addr = inet_addr(ip);
    }

    return server_address;
}

int create_socket(unsigned int port) {
    int socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in udp_load_balancer_address = get_server_address(NULL, port);

    bind(
        socket_,
        (struct sockaddr*)&udp_load_balancer_address,
        sizeof(udp_load_balancer_address)
    );

    return socket_;
}

void listen_udp_packet(
    char message[MAX_PAYLOAD_LENGTH],
    unsigned int *message_length,
    int socket_
) {
    unsigned int client_address_length;
    struct sockaddr_in client_address;

    *message_length = recvfrom(
        socket_, message,
        MAX_PAYLOAD_LENGTH,
        0,
        (struct sockaddr *) &client_address,
        &client_address_length
    );
}

void send_udp_packet(
    char *ip,
    int port,
    char message[MAX_PAYLOAD_LENGTH],
    int message_length,
    int socket
) {
    struct sockaddr_in destination_server = get_server_address(ip, port);

    sendto(socket, message, message_length, 0, (struct sockaddr*)&destination_server, sizeof(destination_server));
}
