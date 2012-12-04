/* 
 * Roberto Acevedo
 * Multi-threaded socket server
 * COP4610
 * server.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>

/* Preprocessor Directives */

#define NTHREADS 50
#define QUEUE_SIZE 5
#define BUFFER_SIZE 256

/* Global counter locked via mutex */

pthread_t threadid[NTHREADS]; // Thread pool
pthread_mutex_t lock;
int counter = 0;

void *threadworker(void *arg)
{

  int sockfd, rw; // File descriptor and 'read/write' to socket indicator
  char *buffer; // Message buffer
  sockfd = (int) arg; // Getting sockfd from void arg passed in

  buffer = malloc(BUFFER_SIZE);
  bzero(buffer, BUFFER_SIZE);

  rw = read(sockfd, buffer, BUFFER_SIZE); // Blocks until there is something to be read in the socket

  if (rw < 0)
  {
    perror("Error reading form socket, exiting thread");
    pthread_exit(0);
  }

  printf("New message received: %s", buffer); // String already has newline
  bzero(buffer, BUFFER_SIZE);
  sprintf(buffer, "Acknowledgement from TID:0x%x", pthread_self()); 

  rw = write(sockfd, buffer, strlen(buffer)); 

  if (rw < 0)
  {
    perror("Error writing to socket, exiting thread");
    pthread_exit(0);
  }

  /* Critical section */

  printf("Requesting mutex lock...\n");
  pthread_mutex_lock (&lock); 
  printf("Current counter value: %d, upping by 1...\n", counter);
  counter++;
  pthread_mutex_unlock (&lock);
  printf("Done! Mutex unlocked again, new counter value: %d\n", counter);

  close(sockfd);
  printf("TID:0x%x served request, exiting thread\n", pthread_self());
  pthread_exit(0);

}


int main(int argc, char *argv[])
{

  /* Variable declarations */

  int serv_sockfd, new_sockfd; //Socket identifiers for server and incoming clients
  struct addrinfo flags; // Params used to establish listening socket
  struct addrinfo *host_info; // Resultset for localhost address info, set by getaddrinfo()

  socklen_t addr_size; // Client address size since we use sockaddr_storage struct to store
                       // client info coming in, not using addrinfo as done for host (local) 
                       // by calling getaddrinfo for resolution, which stores results in 
                       // the more convenient addrinfo struct

  struct sockaddr_storage client; // Sockaddr storage struct is larger than sockaddr_in, 
                                  // can be used both for IPv4 and IPv6

  pthread_attr_t attr; // Thread attribute
  int i; // Thread iterator

  /* Start of main program */

  if (argc < 2) {
    fprintf(stderr,"Error: no port provided\n");
    exit(-1);
  }

  memset(&flags, 0, sizeof(flags));
  flags.ai_family = AF_UNSPEC; // Use IPv4 or IPv6, whichever
  flags.ai_socktype = SOCK_STREAM; // TCP
  flags.ai_flags = AI_PASSIVE; // Set address for me 

  if (getaddrinfo(NULL, argv[1], &flags, &host_info) < 0)
  {
    perror("Couldn't read host info for socket start");
    exit(-1);
  }

  serv_sockfd = socket(host_info->ai_family, host_info->ai_socktype, host_info->ai_protocol);

  if (serv_sockfd < 0)
  {
    perror("Error opening socket");
    exit(-1);
  }

  if (bind(serv_sockfd, host_info->ai_addr, host_info->ai_addrlen) < 0) 
  {
    perror("Error on binding");
    exit(-1);
  } 

  freeaddrinfo(host_info); // Don't need this struct anymore

  pthread_attr_init(&attr); // Creating thread attributes
  pthread_attr_setschedpolicy(&attr, SCHED_FIFO); // FIFO scheduling for threads 
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // Don't want threads (particualrly main)
                                                               // waiting on each other


  listen(serv_sockfd, QUEUE_SIZE); // Pass in socket file descriptor and the size of the backlog queue 
                                   // (how many pending connections can be in queue while another request
                                   // is handled)

  addr_size = sizeof(client);
  i = 0;

  while (1) 
  {
    if (i == NTHREADS) // So that we don't access a thread out of bounds of the thread pool
    {
      i = 0;
    }
    
    new_sockfd = accept(serv_sockfd, (struct sockaddr *) &client, &addr_size);

    if (new_sockfd < 0) 
    {
      perror("Error on accept");
      exit(-1);
    }

    pthread_create(&threadid[i++], &attr, &threadworker, (void *) new_sockfd);
    sleep(0); // Giving threads some CPU time
  }

  return 0; 
}
