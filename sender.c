#include "arq_packet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <errno.h>

int  openTCPConnection (const char * const ip, const int port);
void load_next_packet(packet *pkt, int packet_number, char *message);
void TwoWayComm(const int sock);
void shift_array(int *array, int size, int amount);
void send_data(const int sock, char *message);

const char payload[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const int TIMEOUT = 3;   //3s time-out

int main(int argc, char *argv[]) {
	if(argc != 4) {
		printf("Please pass the IP address, port number and BER value, e.g., ./sender 127.0.0.1 50404 0.001");
		exit(-1);
	}
	char   *remote_ip  = argv[1]; 
	int    remote_port = atoi(argv[2]);
	double ber         = atof(argv[3]);
	int    sender_sock = openTCPConnection(remote_ip, remote_port);
	//Write your code here
	
  send_data(sender_sock, payload);
	
	//End of your code
	
	close(sender_sock);
	exit(0);
}

int openTCPConnection(const char * const remote_ip, const int remote_port) {
	/* Create socket to connect remote hosts */
	int sock;
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket() error:\n");
		exit(-1);
	}
	/* Construct the address structure of the remote host */
	struct sockaddr_in remote_addr;
	remote_addr.sin_family = PF_INET;
	remote_addr.sin_port = htons(remote_port);
	remote_addr.sin_addr.s_addr = inet_addr(remote_ip);
	/* Connect to the remote host */
	if(connect(sock, (struct sockaddr *) &remote_addr, sizeof(remote_addr)) < 0) {
		perror("connect() failed.\n");
		exit(-1);
	} 
	//Set the timeout of the socket
	struct timeval tv;
	tv.tv_sec  = TIMEOUT;
	tv.tv_usec = 0;
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0) {
		perror("setsockopt() failed.\n");
		exit(-1);
	}
	return(sock);
}

void TwoWayComm(const int sock) {
	int  read_size;
    char msg[100], reply[150];
    while(1) {
        printf("Enter message : ");
		fgets(msg, 100 , stdin);
        
		// Send some data to the receiver
		// msg         - the message to be sent
		// strlen(msg) - length of the message
		// If return value < 0, an error occurred.
        if(write(sock, msg, strlen(msg)) < 0)
            perror("write() failed.\n");
        
		// Receive a reply from the receiver
		// NOTE: If you have more data than 149 bytes, it will be received in the next call to "read"
		// read_size - the length of the received message or -1 indicating an error
		// reply     - a buffer where the received message will be stored
		// 149       - the size of the receiving buffer (any more data will be delivered in subsequent "read" operations
        if((read_size = read(sock, reply, 149)) < 0) { 
		    int errsv = errno;
			if (errsv == EWOULDBLOCK) {
				//If the sender doesn't get any reply in TIMEOUT time, read() returns -1 and "errno" is set to EWOULDBLOCK.
				perror("read() time out.\n");  
			} else {
				perror("read() failed, other errors.\n");
			}
		}
 		// Null termination since we need to print it
		reply[read_size] = '\0';
        printf("Receiver's reply: %s\n", reply);
    }	
}

void send_data(const int sock, char *message) {
  int read_size;
  int buffer_len = 0;
  int state  = NOT_DONE;
  int window = 0;
  int packet_number = 0;
  int i = 0;

  int msg_size = strlen(message);

  int window_sent[WINDOW_SIZE];

  for(i = 0; i < WINDOW_SIZE; i++) {
    window_sent[i] = 0;
  }

  packet pkt = {};
  packet last_received = {};

  while(state == NOT_DONE) {
    for(i = 0; i < WINDOW_SIZE && i < msg_size/2 - packet_number; i++) {
        load_next_packet(&pkt, packet_number + i, message);
        printf("Sending Packet: Type: %d, Number: %d, Data: %c %c\n", pkt.type, pkt.number, pkt.data[0], pkt.data[1]);
        send_packet(sock, &pkt);
    }

    ioctl(sock, FIONREAD, &buffer_len);

    // Get replies from the receiver if they exist
    while(buffer_len > 0) {
      receive_packet(sock, &last_received, &read_size);
      if(last_received.type == ACK) {
        window_sent[last_received.number - packet_number] = 2;
        printf("Receiver's reply: Type: %d, Number: %d\n", last_received.type, last_received.number);
      } else if(last_received.type == NAK){
        window_sent[last_received.number - packet_number] = -1;
      }
      ioctl(sock, FIONREAD, &buffer_len);
    }
    int shift_by = 0;

    //Check for which packets have been ack'd
    //move the window accordingly
    for(i = 0; i < WINDOW_SIZE; i++){
      if(window_sent[i] == -1){
        break;
      } else if(window_sent[i] == 2){
        packet_number++;
        window_sent[i] = 0;
        shift_by++;
      }
    }

    shift_array(&window_sent, WINDOW_SIZE, shift_by);

    //Have we sent all the stuff yet?
    if(packet_number >=  msg_size/2){
      state = DONE;
    }
  }
}

void load_next_packet(packet *pkt, int packet_number, char *message) {
  fill_packet(pkt, DATA, packet_number, message[packet_number*2], message[packet_number*2 + 1]);
}

void shift_array(int *array, int size, int amount) {
  for(int i = size; i > amount; i--) {
    array[i] = array[i-1];
  }
}
