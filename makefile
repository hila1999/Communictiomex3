# Compiler and flags for both projects
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS =

# Targets for the TCP project
TCP_RECEIVER_TARGET = TCP_Receiver
TCP_SENDER_TARGET = TCP_Sender

# Object files for the RUDP project
RUDP_SENDER_OBJECTS = RUDP_Sender.o RUDP_API.o
RUDP_RECEIVER_OBJECTS = RUDP_Receiver.o RUDP_API.o

# Combined target for all
all: $(TCP_RECEIVER_TARGET) $(TCP_SENDER_TARGET) RUDP_Sender RUDP_Receiver

# Rules for building executables for the TCP project
$(TCP_RECEIVER_TARGET): TCP_Receiver.c
	$(CC) $(CFLAGS) -o $(TCP_RECEIVER_TARGET) TCP_Receiver.c

$(TCP_SENDER_TARGET): TCP_Sender.c
	$(CC) $(CFLAGS) -o $(TCP_SENDER_TARGET) TCP_Sender.c

# Rules for building executables for the RUDP project
RUDP_Sender: $(RUDP_SENDER_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(RUDP_SENDER_OBJECTS)

RUDP_Receiver: $(RUDP_RECEIVER_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(RUDP_RECEIVER_OBJECTS)

# Rules for building object files for the RUDP project
RUDP_Sender.o: RUDP_Sender.c RUDP_API.h
	$(CC) $(CFLAGS) -c -o $@ RUDP_Sender.c

RUDP_Receiver.o: RUDP_Receiver.c RUDP_API.h
	$(CC) $(CFLAGS) -c -o $@ RUDP_Receiver.c

RUDP_API.o: RUDP_API.c RUDP_API.h
	$(CC) $(CFLAGS) -c -o $@ RUDP_API.c

# Cleaning rule
clean:
	rm -f $(TCP_RECEIVER_TARGET) $(TCP_SENDER_TARGET) RUDP_Sender RUDP_Receiver *.o
