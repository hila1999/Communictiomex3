#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#define TCP_CONGESTION 1
#define PORT 8080

int main() {
  int sockfd, newsockfd;
  struct sockaddr_in servaddr, cliaddr;
  char buffer[1024];
  int n, bytes_received;
  char filename[100];
  FILE *fp;
  clock_t start, end;
  double time_taken, avg_time, total_time = 0, total_bytes = 0, avg_bandwidth;

  // Create socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(1);
  }

  // Set TCP congestion control algorithm
  setsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, "reno", 5);

  // Fill server address structure
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = INADDR_ANY;

  // Bind socket to address
  if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind");
    exit(1);
  }

  // Listen for connections
  listen(sockfd, 5);

  printf("Starting Receiver...\n");

  while (1) {
    // Accept new connection
    newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, (socklen_t *)&cliaddr);
    if (newsockfd < 0) {
      perror("accept");
      exit(1);
    }

    printf("Sender connected, beginning to receive file...\n");

    // Set TCP congestion control algorithm for new socket
    setsockopt(newsockfd, IPPROTO_TCP, TCP_CONGESTION, "cubic", 5);

    // Receive filename
    n = recv(newsockfd, filename, sizeof(filename), 0);
    if (n < 0) {
      perror("recv");
      exit(1);
    }
    filename[n] = '\0'; // Null terminate the filename

    // Open file
    fp = fopen(filename, "wb");
    if (fp == NULL) {
      perror("fopen");
      exit(1);
    }

    // Start time measurement
    start = clock();

    // Receive file data
    while ((bytes_received = recv(newsockfd, buffer, sizeof(buffer), 0)) > 0) {
      fwrite(buffer, 1, bytes_received, fp);
      total_bytes += bytes_received;
    }

    // End time measurement
    end = clock();

    // Calculate time taken to receive file
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    total_time += time_taken;

    // Close file
    fclose(fp);

    printf("File transfer completed.\n");

    // Print time taken and bandwidth
    printf("Time=%fms; Speed=%fMB/s\n", time_taken * 1000, total_bytes / time_taken / (1024 * 1024));

    // Send confirmation message
    send(newsockfd, "ok", 2, 0);

    // Close new socket
    close(newsockfd);

    // Calculate averages
    avg_time = total_time / 1; // Assuming one run here
    avg_bandwidth = total_bytes / total_time / (1024 * 1024);

    printf("Waiting for Sender response...\n");

    // Receive confirmation from sender
   
  n = recv(newsockfd, buffer, sizeof(buffer), 0);
if (n < 0) {
  perror("recv");
  exit(1);
}

// Close new socket
close(newsockfd);

  }
\
  return 0;
}
