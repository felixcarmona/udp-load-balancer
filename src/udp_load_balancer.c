#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "udp_load_balancer.h"
#include "argparse.h"
#include "socket.h"
#include "hostname_resolver.h"


unsigned int get_servers_amount(char **servers_list) {
    unsigned int i = 0;

    while(1) {
        if(servers_list[i] == NULL) {
            return i;
        }
        i++;
    }
}

void run_dispatcher(char **servers_list, int socket_)
{
    char message[MAX_PAYLOAD_LENGTH];
    unsigned int message_length;

    unsigned int servers_amount = get_servers_amount(servers_list);

    char **servers_hosts = get_servers_hosts(servers_list, servers_amount);
    int *servers_ports = get_servers_ports(servers_list, servers_amount);

    unsigned int i;

    for (i = 0; i < servers_amount; i++) {
        if (is_ip(servers_hosts[i]) == 0) {
            servers_hosts[i] = get_ip_from_hostname(servers_hosts[i]);
        }
    }

    while(1) {
        for(i = 0; i < servers_amount; i++) {
            listen_udp_packet(message, &message_length, socket_);

            send_udp_packet(servers_hosts[i], servers_ports[i], message, message_length, socket_);
        }
    }
}

void show_help() {
    printf("\n"
        "Usage: udp_load_balancer [-h] [--port PORT] [--servers SERVERS]\n"
        "\n"
        "The options are as follows:\n"
        "      -h, --help            show this help message and exit\n"
        "      -p, --port PORT\n"
        "                            Port to listen UDP messages (min 1, max 65535)\n"
        "      -s, --servers SERVERS\n"
        "                            Servers list to balance the UDP messages\n"
        "                            Example: \"127.0.0.1:8123, localhost:8124, example.com:8123\"\n"
    );
}

int main(int argc, char **argv)
{
    if (is_help_required(argc, argv)) {
        show_help();
        exit(0);
    }

    unsigned int udp_load_balancer_port = get_udp_load_balancer_port(argc, argv);
    char **servers_list  = get_servers_list(argc, argv);

    if (udp_load_balancer_port == 0 || servers_list == NULL) {
        show_help();
        exit(2);
    }

    unsigned int servers_amount = get_servers_amount(servers_list);
    printf("Listening on port: %d\n", udp_load_balancer_port);

    printf("Balancing to servers:\n");
    char **servers_hosts = get_servers_hosts(servers_list, servers_amount);
    int *servers_ports = get_servers_ports(servers_list, servers_amount);
    unsigned int i;
    for (i = 0; i < servers_amount; ++i) {
        printf("    %s", servers_list[i]);
        if(is_ip(servers_hosts[i]) == 0) {
            printf(" (%s:%d)", get_ip_from_hostname(servers_hosts[i]), servers_ports[i]);
        }
        printf("\n");
    }

    int socket_ = create_socket(udp_load_balancer_port);

    run_dispatcher(servers_list, socket_);

    return 0;
}
