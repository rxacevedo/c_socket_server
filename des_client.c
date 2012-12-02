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

#define BUFFER_SIZE 256

void error(const char *msg)
{
  perror(msg);
  exit(0);
}

int main(int argc, char *argv[])
{
  int sockfd, portno, rw;
  struct sockaddr_in *destination;
  struct addrinfo hints;
  struct addrinfo *server_info;

  char* message = malloc(sizeof(char)*140); // Twitter character limit 
  // char dest_address[256] = "10.0.1.20";
  // server = inet_addr("10.0.1.20");
  
  memset(&hints, 0, sizeof(hints)); // Clear to we're not working with garbage
  hints.ai_family = AF_UNSPEC; // IPv4 or IPv6 doesn't matter
  hints.ai_socktype = SOCK_STREAM; // TCP
  hints.ai_flags = AI_PASSIVE; // get the IP for me
  portno = 8080;

  // Initialize socket and check it
  // if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
  if (getaddrinfo(argv[1], "8080", &hints, &server_info) < 0) {
    error("Couldn't find host.");
    exit(0);
  }

  sockfd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

  // connect to server
  bzero(&destination, sizeof(&destination)); // Zero server address to prevent 
                                                     // it initializing to crap (for the
                                                     // size of the struct)

  destination = (struct sockaddr_in *) server_info->ai_addr;

  // destination.sin_family = AF_INET; // Making Internets
  // destination.sin_addr.s_addr = inet_addr("10.0.1.30");
  // destination.sin_addr.s_addr = inet_addr(server_info->ai_addr);
  // destination.sin_port = htons(portno); // Converting portno to network byte order
  // connect(sockfd, (struct sockaddr *) &destination, sizeof(destination));

  if (connect(sockfd, server_info->ai_addr, server_info->ai_addrlen) < 0)
  {
    error("Couldn't conenct...");
    exit(0);
  } 

  printf("Whaddup...\n");
  bzero(message, 256);
  fgets(message, 255, stdin);

  rw = write(sockfd, message, strlen(message)); // Sending the contents of the buffer - writes using socket file descriptor
  if (rw < 0) // Is this the length of the data sent out, or an stderr?
  {
    error("Failed to send message.");
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
    error("ERROR: Can't read from socket");
    exit(0);
  }

  printf("The message is: %s\n", message);

  close(sockfd);
  return 0;

}
