#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "hostname_resolver.h"


int is_ip(char *address)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, address, &(sa.sin_addr));

    return result != 0;
}

char *get_ip_from_hostname(char *hostname)
{
    struct hostent *he = gethostbyname(hostname);

    if (he == NULL) {
        printf("Host \"%s\" not found\n", hostname);
        exit(1);
    }

    struct in_addr **addr_list = (struct in_addr **)he->h_addr_list;

    /* return the first one, because one host can have multiple addresses */
    return inet_ntoa(*addr_list[0]);
}