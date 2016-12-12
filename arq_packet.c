#include <stdlib.h>
#include <stdio.h> 
#include <string.h>    
#include <sys/socket.h>    
#include "ccitt16.h"
#include "arq_packet.h"

void send_ack(int sockfd, int sequence_number){
  unsigned char send_buff[3];
  send_buff[0] = (sequence_number >> 8) & 0x00ff;
  send_buff[1] = sequence_number & 0x00ff;
  send_buff[2] = '\0';

  if( send(sockfd , send_buff, strlen(send_buff), 0) < 0)
      perror("Send failed");

}

void send_packet(int sockfd, packet *pkt) {
    unsigned char send_buff[100];

    send_buff[0] = pkt->number;
    send_buff[1] = pkt->data[0];
    send_buff[2] = pkt->data[1];

    short int crc = calculate_CCITT16(send_buff, 4, GENERATE_CRC);

    send_buff[3] = (crc >> 8) & 0x00ff;
    send_buff[4] = crc & 0x00ff;

    // Send some data to the receiver
		// msg - the message to be sent
		// strlen(msg) - length of the message
		// 0 - Options and are not necessary here
		// If return value < 0, an error occured
        if( send(sockfd , send_buff, strlen(send_buff), 0) < 0)
            perror("Send failed");
}

int receive_ack(int sockfd) {
    int read_size
    unsigned char reply[150];
    short int sequence_number = -1;
    // Receive a reply from the server
		// NOTE: If you have more data than 149 bytes, it will 
		// 			be received in the next call to "recv"
		// read_size - the length of the received message 
		// 				or an error code in case of failure
		// msg - a buffer where the received message will be stored
		// 149 - the size of the receiving buffer (any more data will be 
		// 		delievered in subsequent "recv" operations
		// 0 - Options and are not necessary here
    if( (read_size = recv(sockfd, reply, 2, 0)) < 0)
        perror("recv failed");

    // Might move back to receiver
    if(read_size == 0)
    {
      puts("Client disconnected");
      fflush(stdout);
      exit(0);
    }
        

    return sequence_number = (reply[0] << 8) | reply[1];
}

int receive_packet(int sockfd, packet *pkt, int *read_size) {
    unsigned char reply[150];
    // Receive a reply from the server
		// NOTE: If you have more data than 149 bytes, it will 
		// 			be received in the next call to "recv"
		// read_size - the length of the received message 
		// 				or an error code in case of failure
		// msg - a buffer where the received message will be stored
		// 149 - the size of the receiving buffer (any more data will be 
		// 		delievered in subsequent "recv" operations
		// 0 - Options and are not necessary here
    if( (read_size = recv(sockfd, reply, sizeof(packet), 0)) < 0)
        perror("recv failed");

    // Might move back to receiver
    if(read_size == 0)
    {
      puts("Client disconnected");
      fflush(stdout);
      exit(0);
    }
        
    short int good_crc = calculate_CCITT16(reply, 4, CHECK_CRC);

    if(good_crc){
      fill_packet(pkt, (unsigned int) reply[0], reply[1], reply[2]);
    } else {
      printf("CRC indicates bad message");
    }

    return good_crc;
}

void fill_packet(packet *pkt, unsigned int number, char data1, char data2){
       pkt->number = number;
       pkt->data[0] = data1;
       pkt->data[1] = data2;
}

void initialize_packet_buff(packet* buffer, int size) {
  for (int i = 0; i < size; i++) {
      fill_packet(&buffer[0], i, '0', '0');
  }
}
