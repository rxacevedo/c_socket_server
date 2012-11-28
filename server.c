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
#include <errno.h>

/* Preprocessor Directives */

#define NTHREADS 100
#define MAX_CONNS 5

/* Globals */

sem_t thread_sem[NTHREADS];
int newsockfd;

void error(const char *msg)
{
  perror(msg);
  exit(1);
}

/* Thread routine */

void *request_handler(void *arg)
{
  int thread_socket;
  thread_socket = newsockfd;
}

/* Main program */
int main(int argc, char *argv[])
{
  int sockfd, portno;
  socklen_t clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  pthread_attr_t attr;
  pthread_t threads;
  int n;
  unsigned int request_args;

  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }

  sockfd = socket(AF_INET, SOCK_STREAM, 0); // Passing in Internet Domain, socket type, and
  // protocol arg (0 lets system decide, uses TCP 
  // for S. We listen on this socket.

  if (sockfd < 0)
  { 
    error("ERROR opening socket");
  }

  bzero((char *) &serv_addr, sizeof(serv_addr)); // Initializing serving address to 0 using 
  // bzero(), pass in pointer to buffer
  // and size of buffer

  portno = atoi(argv[1]); // Getting the port number from argv (command line argument)

  serv_addr.sin_family = AF_INET; // Setting values in serv_addr struct with lib supplied constants
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  if (bind(sockfd, (struct sockaddr *) &serv_addr,
        sizeof(serv_addr)) < 0) 
  {
    error("ERROR on binding");
  }

  listen(sockfd, MAX_CONNS); // Pass in socket file descriptor and the size of the backlog queue 
  // (how many pending connections can be in queue while another request
  // is handled)

  pthread_attr_init(&attr); // Creating thread attributes

  clilen = sizeof(cli_addr);

  newsockfd = accept(sockfd, // Block until we get a request 
      (struct sockaddr *) &cli_addr, 
      &clilen);


  if (newsockfd < 0) 
  {
    error("ERROR on accept");
  } else {

    /* SPAWN NEW THREAD HERE */
    request_args = newsockfd;
    pthread_create(&threads, &attr, request_handler, &request_args);

  }

  bzero(buffer,256);
  printf("errno prior to bogus socket creation: %s", strerror(errno));
  perror("");
  newsockfd = 2034032403240325023;
  n = read(newsockfd,buffer,255); // Blocks until there is something to be read in the socket
  printf("%d", n);
  // n = -56; 
  if (n < 0) 
  {
    int errno_save = errno;
    puts("Socket read error!"); // Use printf, errno can be modified by printf,
    // perror, and other functions, so errno's value
    // may have changed prior to perror's call, making
    // error codes inaccurate.
    // error("HELLO");
    // return SO_ERROR;
    printf("Error code: %d", errno_save);
    exit(1);
  } else 
  {
    printf("New message: %s\n",buffer);
    n = write(newsockfd,"Message received.",18);
    if (n < 0) error("Socket write error!");
    close(newsockfd);
    close(sockfd);
    return 0;
  } 
}
