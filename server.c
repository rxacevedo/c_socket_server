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
#include <netdb.h>

/* Preprocessor Directives */

#define NTHREADS 100
#define QUEUE_SIZE 5
#define BUFFER_SIZE 256
#define TRUE 1

/* Globals */

// sem_t thread_sem[NTHREADS];
pthread_mutex_t lock;
int counter = 0;

void *threadalizer(void *arg)
{
  int sockfd, rw; // File descriptor and 'read/write' to socket indicator
  char *buffer;
  sockfd = (int) arg;

  buffer = malloc(BUFFER_SIZE);
  bzero(buffer, BUFFER_SIZE);
  rw = read(sockfd, buffer, sizeof(buffer)-1); // Blocks until there is something to be read in the socket

  if (rw < 0) perror("ERROR reading from socket");

  printf("Here is the message: %s\n", buffer);
  bzero(buffer, sizeof(buffer)-1);

  sprintf(buffer, "Acknowledgement from thread 0x%x", pthread_self()); // Thread IDs aren't meaningfully 
                                                                       // castable since they are opaque 
                                                                       // objects, but this at least provides
                                                                       // some way to identify threads

  rw = write(sockfd, buffer, sizeof(buffer)-1); // Minus one so as to not read null terminator

  if (rw < 0) perror("ERROR writing to socket");

  pthread_mutex_lock (&lock); // Critical area
  counter++;
  pthread_mutex_unlock (&lock);

  close(sockfd);
  printf("Request for thread 0x%x served.\n", pthread_self());
  pthread_exit(0);
}


int main(int argc, char *argv[])
{

  int serv_sockfd, new_sockfd;
  //  , portno;
  struct addrinfo flags;
  struct addrinfo *host_info;
  socklen_t addr_size; // TODO: Don't need this, use addrinfo->ai_addrlen instead!
  // struct sockaddr_in serv_addr, cli_addr; // Don't need these either
  struct sockaddr_storage client;
  pthread_attr_t attr;
  pthread_t threadid[NTHREADS];

  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(-1);
  }

  // HOST INFO HERE
  if (getaddrinfo(NULL, argv[1], &flags, &host_info) != 0)
  {
    perror("Couldn't read host info for socket start");
    exit(-1);
  }

  serv_sockfd = socket(host_info->ai_family, host_info->ai_socktype, host_info->ai_protocol);

  if (serv_sockfd < 0) perror("ERROR opening socket");

  // bzero((char *) &serv_addr, sizeof(serv_addr)); // Initializing serving address to 0 using 
                                                    // bzero(), pass in pointer to buffer
                                                    // and size of buffer

  // portno = atoi(argv[1]); // Getting the port number from argv (command line argument)
  // serv_addr.sin_family = AF_INET; // Setting values in serv_addr struct with lib supplied constants
  // serv_addr.sin_addr.s_addr = INADDR_ANY;
  // serv_addr.sin_port = htons(portno);

  if (bind(serv_sockfd, host_info->ai_addr, host_info->ai_addrlen) < 0) 
  {
    perror("ERROR on binding");
    exit(-1);
  }

  // printf("Server started, listening for connections...");

  while (TRUE) 
  {
    listen(serv_sockfd, QUEUE_SIZE); // Pass in socket file descriptor and the size of the backlog queue 
                                     // (how many pending connections can be in queue while another request
                                     // is handled)

    pthread_attr_init(&attr); // Creating thread attributes
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO); // I want FIFO - test
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    addr_size = sizeof(client);
    new_sockfd = accept(serv_sockfd, (struct sockaddr *) &client, &addr_size);

    if (new_sockfd < 0) perror("ERROR on accept");

    pthread_create(&threadid, &attr, threadalizer, (void *) new_sockfd);
    // pthread_join(threadid, NULL);
    printf("Back in main thread: 0x%x\n", pthread_self());

  }

  return 0; 
}
