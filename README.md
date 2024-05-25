# network-sockets

(12.01.2023)

Part 1. Building a network server. Make a program that: 

1. Performs daemonization, further functionality concerns the daemon. 
2. Describes the global descriptor of the log file. 
3. Describes the struct sockaddr_in structure with parameters: socket format - PF_INET, address 
- any, port - 3200 + 16. 
4. Forms a SOCK_STREAM socket of the PF_INET format. 
5. Configures the socket to wait for requests using bind.
6. Starts an endless cycle of processing requests from customers. 
7. A fork is executed for each request to generate a processing process. 
8. The parent process closes the socket. 
9. The processing process in an endless loop receives deadlines from the client using recv, 
adds its prefix in the form of the current time and its own pid to them, and returns 
to the client using send. 
10. The processing process completes the processing of data from the client after receiving the term "close". 
11. Each action of the server is accompanied by a note in the log.
  
Part 2. Building a network client. Make a program that: 

1. Describes the struct sockaddr_in structure with parameters: socket format - PF_INET, address 
- calculated by calling htonl(INADDR_LOOPBACK), port - 3200 + 16. 
2. Forms a SOCK_STREAM socket of the PF_INET format. 
3. Configures the socket to connect to the server using connect.
4. In an endless loop, requests lines from the operator, transfers them to the server and prints them 
received answers.
5. The work ends after the operator enters the line "close" and receives it 
responses from the server.

Investigate the operation of the network server. Research by running the server and several instances of the client in different sessions. Analyze the server log and draw conclusions about the moment of startup and shutdown connection processing processes.
