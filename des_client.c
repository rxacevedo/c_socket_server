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

// typedef struct {
//   int sockfd;
//   char* message;
// } ThreadData

// void *threadalizer(void *arg);
// void error(const char *msg);

void error(const char *msg)
{
  perror(msg);
  exit(0);
}

// void *threadalizer(void *arg)
// {
//   rw = 1;
// }

int main()
{
  int sockfd, portno, rw;
  struct sockaddr_in destination;
  char* message = malloc(256); 
  // char dest_address[256] = "10.0.1.20";

  pthread_t threadid;
  pthread_attr_t attr;

  // server = inet_addr("10.0.1.20");
  portno = 8080;

  // Initialize socket and check it
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    error("Error opening socket");
    exit(0);
  }


  // connect to server
  bzero((char *) &destination, sizeof(destination)); // Zero server address to prevent 
                                                     // it initializing to crap (for the
                                                     // size of the struct)

  destination.sin_family = AF_INET; // Making Internets
  destination.sin_addr.s_addr = inet_addr("192.168.1.7");

  destination.sin_port = htons(portno); // Converting portno to network byte order

  connect(sockfd, (struct sockaddr *) &destination, sizeof(destination));

  if (connect(sockfd, (struct sockaddr *) &destination, sizeof(destination))< 0)
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
