Roberto Acevedo
COP4610
USF

[![Build Status](https://travis-ci.org/rxacevedo/c_socket_server.png?branch=master)](https://travis-ci.org/rxacevedo/c_socket_server)

This is a C implementation of a client/server environment using POSIX threads and IPv4/IPv6
complient sockets. It was a project that I did for an IT Operating Systems class in Fall 2012, and my first real endeavor in C. 

Server:
Usage: ./server <portno>

Each time a connection is made via the listen socket, a new thread is spawned and it reads the 
message from the client out of the buffer. The server prints the message, clears the buffer, 
then closes the connection and kills the thread, returning to main. The accept() call is 
nested within a continuous while loop (inefficient I know, I need to move to an interrupt-based solution), so incoming connections can always be served.

Client:
Usage: ./client <hostname/address> <port>

The client attempts to establish a connection to the server. On success, it prompts the user
for a message to send to the server, (stored in buffer), then sends it to the server. The server
then writes a response via the socket connection back to the client, acknowledging that the
request was server.

