#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define FILE_PATH "random_file.txt"

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void createRandomFile() {
    // Use dd command to create a random file of approximately 2 MB
    system("dd if=/dev/urandom of=random_file.txt bs=1024 count=2000");
}

int main(int argc, char *argv[]) {
    if (argc != 7) {
        fprintf(stderr, "Usage: %s -ip <IP> -p <Port> -algo <ALGO>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *receiver_ip = NULL;
    int receiver_port = -1;
    const char *algo = NULL;

    // Parsing command line arguments
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-ip") == 0) {
            receiver_ip = argv[i + 1];
        } else if (strcmp(argv[i], "-p") == 0) {
            receiver_port = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-algo") == 0) {
            algo = argv[i + 1];
        }
    }

    // Checking if required arguments are provided
    if (receiver_ip == NULL || receiver_port == -1 || algo == NULL) {
        fprintf(stderr, "Receiver IP, port number, or algorithm is missing or invalid.\n");
        exit(EXIT_FAILURE);
    }

    printf("Creating a random file...\n");
    createRandomFile();
    printf("Random file created.\n");

    printf("Sender started. Connecting to %s on port %d with algorithm %s.\n", receiver_ip, receiver_port, algo);
    
    // Creating a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("Error opening socket");

    // Setting the TCP congestion control algorithm
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, algo, strlen(algo)) < 0)
        error("Error setting congestion control algorithm");

    // Initializing the receiver's address structure
    struct sockaddr_in receiver_addr;
    memset(&receiver_addr, 0, sizeof(receiver_addr));
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(receiver_port);

    // Converting receiver's IP address to binary form
    if (inet_pton(AF_INET, receiver_ip, &receiver_addr.sin_addr) <= 0)
        error("Error converting IP address");

    // Establishing a connection with the receiver
    if (connect(sockfd, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr)) < 0)
        error("Error connecting to receiver");

    printf("Connection established with the Receiver.\n");

    while (1) {
        FILE *file = fopen(FILE_PATH, "rb");
        if (!file)
            error("Error opening file");

        char buffer[BUFFER_SIZE];
        size_t bytesRead;

        // Preparing the receiver
        send(sockfd, "prepare", 7, 0);
        
        // Sending the file
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            send(sockfd, buffer, bytesRead, 0);
        }

        fclose(file);

        // Notifying the receiver that the file has been sent
        send(sockfd, "sent", 4, 0);

        // Asking the user if they want to send the file again
        char response;
        printf("Do you want to send the file again? (y/n): ");
        scanf(" %c", &response);

        if (response != 'y') {
            if (response == 'n') {
                // Sending exit message to the receiver
                send(sockfd, "exit", 4, 0);
                break;
            } else {
                error("Incorrect character entered. Exiting.");
            }
        }
    }

    close(sockfd);
    remove(FILE_PATH);

    return 0;
}
