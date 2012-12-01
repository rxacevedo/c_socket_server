/* A simple server in the internet domain using TCP
   The port number is passed as an argument */

/* Includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>

/* Preprocessor Directives */

#define NTHREADS 100
#define QUEUE_SIZE 5
#define TRUE 1

/* Globals */

sem_t thread_sem[NTHREADS];
pthread_t chkthread;
pthread_mutex_t lock;

void *threadalizer(void *arg)
{
  int fd, rw; // File descriptor and 'read/write' to socket indicator
  char buffer[256];
  fd = (int) arg;

  bzero(buffer,256);
  rw = read(fd, buffer, sizeof(buffer)-1); // Blocks until there is something to be read in the socket

  if (rw < 0) perror("ERROR reading from socket");

  printf("Here is the message: %s\n", &buffer);
  bzero(buffer, sizeof(buffer)-1);

  sprintf(buffer, "Acknowledgement from thread %x", 
    (unsigned int) pthread_self()); // Thread IDs aren't really castable since
                                    // they are opaque objects, but this at least
                                    // proves some way to identify threads

  rw = write(fd, buffer, sizeof(buffer)-1); // Minus one so as to not read null terminator

  if (rw < 0) perror("ERROR writing to socket");
  close(fd);
  printf("Request for thread %lu served.\n", pthread_self());
  pthread_exit(0);
}


int main(int argc, char *argv[])
{
  int serv_sockfd, new_sockfd, portno;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  pthread_attr_t attr;
  pthread_t threadid;

  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }

  serv_sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (serv_sockfd < 0) perror("ERROR opening socket");

  bzero((char *) &serv_addr, sizeof(serv_addr)); // Initializing serving address to 0 using 
                                                 // bzero(), pass in pointer to buffer
                                                 // and size of buffer

  portno = atoi(argv[1]); // Getting the port number from argv (command line argument)
  serv_addr.sin_family = AF_INET; // Setting values in serv_addr struct with lib supplied constants
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  if (bind(serv_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) perror("ERROR on binding");

  while (TRUE) {

    listen(serv_sockfd, QUEUE_SIZE); // Pass in socket file descriptor and the size of the backlog queue 
                                // (how many pending connections can be in queue while another request
                                // is handled)

    pthread_attr_init(&attr); // Creating thread attributes

    clilen = sizeof(cli_addr);
    new_sockfd = accept(serv_sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if (new_sockfd < 0) perror("ERROR on accept");

    pthread_create(&threadid, &attr, threadalizer, (void *) new_sockfd);
    // pthread_join(threadid, NULL);
    printf("Back in main thread: %lu", pthread_self());

  }

  return 0; 
}
