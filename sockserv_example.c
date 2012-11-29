#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define PORT_NO 1234

typedef int Socket;
typedef pthread_t Thread;

void* do_server(void*);
void* do_client(void*);

int main(void) {
    
    Thread other_thread;
    int ret = pthread_create(&other_thread, NULL, do_server, NULL);
    if(ret < 0)
        printf("Trouble creating thread in main...\n");
    
    //  printf("%s", "Going to sleep...\n");
    sched_yield();
    sleep(2);
    //  printf("%s", "Waking up...\n");
    
    do_client(NULL);
    
    pthread_exit(NULL);
    
    return 0;
    
}

void* do_server(void* param) {
    
    printf("%s", "In server code...\n");
    
    Socket my_socket = socket( PF_INET , SOCK_STREAM , 0 );
    if(my_socket < 0)
        printf("%s", "Socket creation failed in server...\n");
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT_NO);
    
    int res = bind( my_socket, (struct sockaddr*) &address, sizeof(struct sockaddr_in));
    
    if(res < 0)
        printf("Trouble binding socket...\n");
    
    res = listen( my_socket, 5 );
    if(res < 0)
        printf("Couldn't listen to socket...\n");
    
    struct sockaddr_in remote_address;
    socklen_t remote_address_len = sizeof(remote_address);
    Socket new_socket;
    
    printf("Accepting...\n");
    new_socket =
    accept( my_socket,  (struct sockaddr*) &remote_address,
           &remote_address_len);
    
    
    if(new_socket < 0) {
        printf("Couldn't accept connection...");
        switch(errno) {
            case EBADF:
                printf("Bad file descriptor\n");
                break;
            case ENOTSOCK:
                printf("Descriptor is not a socket\n");
                break;
            case EOPNOTSUPP:
                printf("Operation not supported\n");
                break;
            case EWOULDBLOCK:
                printf("Would block\n");
                break;
            default:
                printf("Unknown error code: %d\n", errno);
                break;
        }
    }
    
    void* data = malloc(0x0400); /* 1k */
    res = recv(new_socket, data, 1024, 0);
    printf("%d bytes received", res);
    
    printf("%s\n", (char*) data);
    free(data);
    
    shutdown(my_socket, 2);
    shutdown(new_socket, 2);
    
    printf("Exiting server code...\n");
    pthread_exit(NULL);
}

void* do_client(void* param) {
    
    printf("%s", "Running client code...\n");
    
    Socket my_socket = socket( AF_INET , SOCK_STREAM , 0 );
    if(my_socket < 0)
        printf("%s", "Socket creation failed in client...\n");
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons(PORT_NO);
    
    int res = connect( my_socket, (struct sockaddr*) &address, sizeof(struct sockaddr));
    if(res < 0)
        printf("Trouble connecting...\n");
    
    const char MSSG[] = "Hello, there!";
    
    res = send( my_socket, MSSG, sizeof(MSSG), 0);
    printf("%d bytes sent\n", res);
    
    printf("%s", "Going to sleep in client code...\n");
    sched_yield();
    sleep(2);
    
    shutdown(my_socket, 2);
    printf("Client thread exiting...\n");
    pthread_exit(NULL);
    
}