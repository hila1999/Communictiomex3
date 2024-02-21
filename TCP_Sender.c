#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h> // Include errno.h for perror
#include <unistd.h> // for close
#define TCP_CONGESTION 1
#define PORT 8080

int main() {
  int sockfd;
  struct sockaddr_in servaddr;
  char buffer[1024];
  int n, bytes_sent;
  char filename[100];
  FILE *fp;

  // Create socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // Set TCP congestion control algorithm
  if (setsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, "reno", 5) < 0) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  // Fill server address structure
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = INADDR_ANY;

  // Connect to the server
  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("connect");
    exit(EXIT_FAILURE);
  }

  // Get the filename from the user
  printf("Enter filename: ");
  scanf("%s", filename);

  // Open the file
  fp = fopen(filename, "rb");
  if (fp == NULL) {

    perror("fopen");
    exit(EXIT_FAILURE);
  }

  // Send the filename to the server
  n = send(sockfd, filename, strlen(filename), 0);
  if (n < 0) {
    perror("send");
    exit(EXIT_FAILURE);
  }
    

  // Send the file to the server
  while ((bytes_sent = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
    n = send(sockfd, buffer, bytes_sent, 0);
    if (n < 0) {
        printf("err1");

      perror("send");
      exit(EXIT_FAILURE);
    }
    memset(buffer, 0, sizeof(buffer)); // Clear buffer
  }
printf("err3");
  // Close the file
  fclose(fp);

  // Send exit message
  if (send(sockfd, "exit", 4, 0) < 0) {
    printf("err2");
    perror("send");
    exit(EXIT_FAILURE);
  }
  // Close the socket
  // shutdown(sockfd,1);
  close(sockfd);
  return 0;
}
