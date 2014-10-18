unsigned int is_help_required(int argc, char **argv);

unsigned int get_udp_load_balancer_port(int argc, char **argv);

char **get_servers_list(int argc, char **argv);

char **get_servers_hosts(char **servers_list, int servers_amount);

int *get_servers_ports(char **servers_list, int servers_amount);