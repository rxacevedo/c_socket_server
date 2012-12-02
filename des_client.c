// Client
// Roberto Acevedo
// COP4610
// Project

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h> // For inet_addr(string arg);
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE sizeof(char)*140

int main(int argc, char *argv[])
{
  int sockfd, rw;
  // struct sockaddr_in *destination;
  struct addrinfo hints;
  struct addrinfo *server_info;

  char* message = malloc(sizeof(char)*140); // Twitter character limit 

  if (argc < 2) perror("Usage: ./server <hostname/address> <port/service type>");
  
  memset(&hints, 0, sizeof(hints)); // Clear to we're not working with garbage
  hints.ai_family = AF_UNSPEC; // IPv4 or IPv6 doesn't matter
  hints.ai_socktype = SOCK_STREAM; // TCP
  hints.ai_flags = AI_PASSIVE; // get the IP for me

  // Try and resolve host from argv[1]

  if (getaddrinfo(argv[1], argv[2], &hints, &server_info) < 0) {
    perror("Couldn't find host");
    exit(0);
  }

  // Initialize socket and check it
  sockfd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

  // Connect to server
  // bzero(&destination, sizeof(&destination)); // Zero server address to prevent 
                                             // it initializing to crap (for the
                                             // size of the struct)

  // destination = (struct sockaddr_in *) server_info->ai_addr;

  if (connect(sockfd, server_info->ai_addr, server_info->ai_addrlen) < 0)
  {
    perror("Couldn't conenct...");
    exit(0);
  } 

  printf("Whaddup...\n");
  bzero(message, 256);
  fgets(message, 255, stdin);

  rw = write(sockfd, message, strlen(message)); // Sending the contents of the buffer - writes using socket file descriptor
  if (rw < 0) // Is this the length of the data sent out, or an stderr?
  {
    perror("Failed to send message.");
  }

  // get & process response
  bzero(message, 256);
  rw = read(sockfd, message, 255); // Reading FROM socket file descriptor
                                   // into the message buffer FOR the size 
                                   // of message buffer MINUS ONE, otherwise
                                   // the null terminator at the end will
                                   // blank out the message

  if (rw < 0)
  {
    perror("ERROR: Can't read from socket");
    exit(0);
  }

  printf("The message is: %s\n", message);

  close(sockfd);
  return 0;

}
