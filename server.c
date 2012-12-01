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
#define TRUE 1

/* Globals */

sem_t thread_sem[NTHREADS];

void error(const char *msg)
{
  perror(msg);
  exit(1);
}

void *threadalizer(void *arg)
{
  int fd, rw;
  char buffer[256];
  fd = (int) arg;
  bzero(buffer,256);
  rw = read(fd ,buffer,255); // Blocks until there is something to be read in the socket
  if (rw < 0) error("ERROR reading from socket");
  printf("Here is the message: %s\n",buffer);
  rw = write(fd,"I got your message",18);
  if (rw < 0) error("ERROR writing to socket");
  close(fd);
  printf("Request for thread %lu served.\n", pthread_self());
  pthread_exit(0);
  // close(sockfd);
  // sprintf(buffer, "The thread is replying to you!");
  // printf("%s", buffer);
  // write(fd, buffer, strlen(buffer));
}


int main(int argc, char *argv[])
{
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  // char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  pthread_attr_t attr;
  pthread_t threadid;
  // int n;

  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0)
  { 
    error("ERROR opening socket");
  }

  bzero((char *) &serv_addr, sizeof(serv_addr)); // Initializing serving address to 0 using 
                                                 // bzero(), pass in pointer to buffer
                                                 // and size of buffer

  portno = atoi(argv[1]);                        // Getting the port number from argv (command line argument)
  serv_addr.sin_family = AF_INET;                // Setting values in serv_addr struct with lib supplied constants
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  if (bind(sockfd, (struct sockaddr *) &serv_addr,
        sizeof(serv_addr)) < 0) 
  {
    error("ERROR on binding");
  }

  while (TRUE) {

    listen(sockfd,5); // Pass in socket file descriptor and the size of the backlog queue 
    // (how many pending connections can be in queue while another request
    // is handled)

    pthread_attr_init(&attr); // Creating thread attributes

    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, 
        (struct sockaddr *) &cli_addr, 
        &clilen);

    if (newsockfd < 0) 
    {
      error("ERROR on accept");
    } else
    {
      pthread_create(&threadid, &attr, threadalizer, (void *) newsockfd);
      pthread_join(threadid, NULL);
      printf("Back in main thread: %lu", pthread_self());
    }
  }

  // bzero(buffer,256);
  // n = read(newsockfd,buffer,255); // Blocks until there is something to be read in the socket
  // if (n < 0) error("ERROR reading from socket");
  // printf("Here is the message: %s\n",buffer);
  // n = write(newsockfd,"I got your message",18);
  // if (n < 0) error("ERROR writing to socket");
  // close(newsockfd);
  // close(sockfd);
  return 0; 
}
