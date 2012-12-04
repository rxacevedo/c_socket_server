/*
 * Roberto Acevedo
 * Multi-threaded socket server project
 * COP4610
 * client.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 256 

int main(int argc, char *argv[])
{
  int sockfd, rw;
  struct addrinfo flags;
  struct addrinfo *server_info;

  char *buffer = malloc(BUFFER_SIZE); 

  if (argc < 3)
  {
    fprintf(stderr, "Usage: ./client <hostname/address> <port>");
    exit(-1);
  }

  memset(&flags, 0, sizeof(flags)); // Clear so we're not working with garbage
  flags.ai_family = AF_UNSPEC; // IPv4 or IPv6 doesn't matter
  flags.ai_socktype = SOCK_STREAM; // TCP
  flags.ai_flags = AI_PASSIVE; // get the IP for me

  if (getaddrinfo(argv[1], argv[2], &flags, &server_info) < 0) { // Resolve host based on CMD args
    perror("Couldn't find host");
    exit(-1);
  }

  sockfd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol); // Initialize socket

  if (connect(sockfd, server_info->ai_addr, server_info->ai_addrlen) < 0)
  {
    perror("Couldn't conenct...");
    exit(-1);
  } 

  printf("Connection established, please enter a message:\n");
  bzero(buffer, BUFFER_SIZE);
  fgets(buffer, BUFFER_SIZE - 1, stdin); 

  rw = write(sockfd, buffer, strlen(buffer)); // Sending the contents of the buffer - writes using socket file descriptor
  if (rw < 0) 
  {
    perror("Failed to send message");
    exit(-1);
  }

  bzero(buffer, BUFFER_SIZE);
  rw = read(sockfd, buffer, BUFFER_SIZE); // Read the ENTIRE buffer because we don't know stlen yet 

  if (rw < 0)
  {
    perror("Error reading from socket");
    exit(-1);
  }

  printf("The message is: %s\n", buffer);

  close(sockfd);
  return 0;

}
