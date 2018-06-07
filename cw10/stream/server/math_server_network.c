//
// Created by farald on 07.06.18.
//

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define NETWORK_CLIENT_NUM_MAX 32

int networkSocketDesc;


void initialize_network(uint16_t port) {
    networkSocketDesc = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sockaddrIn;
    sockaddrIn.sin_family = AF_INET;
    sockaddrIn.sin_port = htobe16(port);
    sockaddrIn.sin_addr.s_addr = INADDR_ANY;
    bind(networkSocketDesc, (struct sockaddr *) &sockaddrIn, sizeof(sockaddrIn));
    listen(networkSocketDesc, NETWORK_CLIENT_NUM_MAX);
}