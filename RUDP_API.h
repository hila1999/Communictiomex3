#ifndef RUDP_API_H
#define RUDP_API_H

#include <stddef.h>
#include <sys/socket.h>
#include <stdint.h>
#include <netinet/in.h>

// Function to create a RUDP socket
int RUDP_socket(struct sockaddr_in *address, int bind_sock);

// Function to send data using RUDP
int RUDP_send(int socket, char *data, size_t length, const struct sockaddr *dest_addr, const struct sockaddr *src_addr, socklen_t addrlen, uint16_t ack_number);

// Function to receive data using RUDP
int RUDP_receive(int socket, void *buffer, const struct sockaddr *addr);

// Function to close RUDP connection
int RUDP_close(int socket, struct sockaddr_in *dest_addr, const struct sockaddr *src_addr, socklen_t addrlen);

#endif
