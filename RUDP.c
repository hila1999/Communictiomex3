#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <fcntl.h>

// הגדרות
#define MAX_PACKET_SIZE 1024
#define RUDP_SYN 1
#define RUDP_SYN_ACK 2
#define RUDP_ACK 3
#define RUDP_FIN 4

// מבנה חבילה
typedef struct rudp_packet {
  uint8_t type;
  uint16_t seq_num;
  uint16_t ack_num;
  char data[MAX_PACKET_SIZE - sizeof(struct rudp_packet)];
} rudp_packet;

// מבנה סוקט RUDP
typedef struct rudp_socket {
  int sock_fd;
  struct sockaddr_in peer_addr;
  uint16_t expected_seq_num;
  uint16_t next_seq_num;
  
} rudp_socket;

rudp_socket *rudp_socket();
int rudp_send(rudp_socket *sock, char *data, int data_len);
char *rudp_recv(rudp_socket *sock);
void rudp_close(rudp_socket *sock);


**rudp_socket* rudp_socket()** {
  // צור סוקט UDP חדש
  int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock_fd < 0) {
    perror("socket");
    return NULL;
  }
  rudp_socket *sock = malloc(sizeof(rudp_socket));
   if (sock == NULL) {
        perror("malloc");
        close(sock_fd);
        return NULL;
    }
    memset(sock, 0, sizeof(rudp_socket)); // Zero out the structure

  sock->sock_fd = sock_fd;
  sock->expected_seq_num = 1;
  sock->next_seq_num = 1;

  return sock;
}

// Function to send data over RUDP
int rudp_send(rudp_socket *sock, char *data, int data_len) {
    int bytes_sent = 0;

    // Split data into packets
    int num_packets = (data_len + MAX_PACKET_SIZE - 1) / MAX_PACKET_SIZE;
    for (int i = 0; i < num_packets; i++) {
        int packet_size = (i == num_packets - 1) ? (data_len % MAX_PACKET_SIZE) : MAX_PACKET_SIZE;
        rudp_packet packet;
        packet.type = RUDP_DATA; // Assuming RUDP_DATA is defined for data packets
        packet.seq_num = sock->next_seq_num++;
        packet.ack_num = 0; // Not used for data packets
        memcpy(packet.data, data + i * MAX_PACKET_SIZE, packet_size);

        // Store the packet in buffer for retransmission (if needed)
        // Implement buffer management logic here
        
        // Send packet through UDP socket
        int bytes_sent_this_packet = sendto(sock->sock_fd, &packet, sizeof(rudp_packet), 0, (struct sockaddr *)&sock->peer_addr, sizeof(sock->peer_addr));
        if (bytes_sent_this_packet == -1) {
            perror("sendto");
            return -1; // Error occurred, return
        }
        bytes_sent += bytes_sent_this_packet;
    }

    // Wait for acknowledgments for each packet within a fixed timeout
    // Implement acknowledgment handling logic here

    return bytes_sent;
}
char *rudp_recv(rudp_socket *sock) {
    // Receive packets from UDP socket
    rudp_packet packet;
    int bytes_received = recvfrom(sock->sock_fd, &packet, sizeof(packet), 0, NULL, NULL);
    if (bytes_received < 0) {
        perror("recvfrom");
        return NULL;
    }

    // Validate packet (checksum, sequence number)
    if (packet.seq_num != sock->expected_seq_num) {
        // Packet out of order, discard it
        return NULL;
    }

    // Store data and update expected sequence number
    char *received_data = malloc(sizeof(char) * (bytes_received - sizeof(rudp_packet) + 1));
    memcpy(received_data, packet.data, bytes_received - sizeof(rudp_packet));
    received_data[bytes_received - sizeof(rudp_packet)] = '\0';
    sock->expected_seq_num++;

    // Send acknowledgment for the received packet
    rudp_packet ack_packet;
    ack_packet.type = RUDP_ACK; // Assuming RUDP_ACK is defined for acknowledgment packets
    ack_packet.seq_num = packet.seq_num;
    ack_packet.ack_num = 0; // Not used for acknowledgment packets
    sendto(sock->sock_fd, &ack_packet, sizeof(ack_packet), 0, (struct sockaddr *)&sock->peer_addr, sizeof(sock->peer_addr));

    return received_data;
}
void rudp_close(rudp_socket *sock) {
    // Close the socket
    close(sock->sock_fd);
    // Free memory allocated for the socket structure
    free(sock);
}