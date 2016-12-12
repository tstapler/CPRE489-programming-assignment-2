#include "helper_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int openTCPConnection(const char * const ip, const int port);
int acceptTCPConnection(const int sock);
void TwoWayComm(const int sock);
void receive_data(const int sock);

int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("Please pass the IP address and port number, e.g., ./receiver 127.0.0.1 50404");
		exit(-1);
	}
	char *local_ip  = argv[1]; 
	int  local_port = atoi(argv[2]);
	int  recv_sock  = openTCPConnection(local_ip, local_port);
	int  accpt_sock = acceptTCPConnection(recv_sock);
	
	//Write your code here
	
	
  printf("The size of packet is %d\n", sizeof(packet));
	
	
	
	// ...
	
	
  receive_data(accpt_sock);
	
	// ...
	
	
	
	
	
	
	//End of your code
	
	close(accpt_sock);	
	close(recv_sock);
	exit(0);
}
int openTCPConnection(const char * const ip, const int port) {
	/* Create socket for incoming connections */
	int sock;
	if((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket() failed.\n") ;
		exit(-1);
	}
	/* Construct local address structure */
	struct sockaddr_in recv_addr;
	recv_addr.sin_family = PF_INET;
	recv_addr.sin_port   = htons(port);
	recv_addr.sin_addr.s_addr = inet_addr(ip);
	/* Bind to the local address */
	if(bind(sock, (struct sockaddr *) &recv_addr, sizeof(recv_addr)) < 0) {
		perror("bind() failed.\n");
		exit(-1);
	}
	/* Listen for connections */
	if(listen(sock, 10) < 0) {
		perror("listen() failed.\n");
		exit(-1);
	}
	return sock;
}
int acceptTCPConnection(const int sock) {
	/* Wait for the sender to connect */
	struct sockaddr_in sender_addr;
	int saddr_len = sizeof(sender_addr);
	int consock; 
	if((consock = accept(sock, (struct sockaddr *) &sender_addr, (socklen_t *) &saddr_len)) < 0) {
		perror("accept() failed.\n");
		exit(-1);
	}
	return consock;
}
void TwoWayComm(const int sock){
	int read_size;
	char msg[150];
    // Receive a message from client
	// read_size - the length of the received message or an error code in case of failure
	// msg - a buffer where the received message will be stored
	// 149 - the size of the receiving buffer (any more data will be delivered in subsequent "read" operations)
	while((read_size = read(sock, msg, 149)) > 0)  {
		// Null termination since we will use strlen(msg) later on and we need to print it now
		msg[read_size] = '\0';
		printf("%s\n", msg);
        // Send the message back to client
		// msg - the message to be sent
		// strlen(msg) - length of the message
		// If return value < 0, an error occurred
        if (write(sock , msg , strlen(msg)) < 0)
        	perror("write() failed.\n");
    }
    if(read_size == 0) {
        puts("Client disconnected");
        fflush(stdout);
    } else if(read_size == -1) {
        perror("recv() failed.\n");
    }
}

void receive_data(const int sock) {
  int read_size = 0;
  int packet_number = 0;
  int buffer_len = 0;

  while(1)  {
    packet last_received = {};
    packet reply = {};

    ioctl(sock, FIONREAD, &buffer_len);
    if(buffer_len > 0) {
      int crc_status = receive_packet(sock, &last_received, &read_size);
        //If all goes well, send back an ACK
        if(crc_status){
            sleep(1);
            printf("Received Packet: Number: %d, Data: %c %c\n",last_received.number, last_received.data[0], last_received.data[1]);
            reply.number = last_received.number;
            packet_number++;
          //This packet had a bad CRC 
        }       
        send_packet(sock, &reply);
    }
  }
}
