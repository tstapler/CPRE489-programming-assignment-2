#include <stdio.h> 
#include <string.h>    
#include <sys/socket.h>    
#include <stdlib.h>

void primary(int sockfd, double ber) {
	int read_size;
    char msg[100], srv_reply[150];

    //keep communicating with server
    while(1)
    {
        printf("Enter message : ");
		fgets(msg, 100 , stdin);
        
        // Send some data to the receiver
		// msg - the message to be sent
		// strlen(msg) - length of the message
		// 0 - Options and are not necessary here
		// If return value < 0, an error occured
        if( send(sockfd , msg, strlen(msg), 0) < 0)
            perror("Send failed");
         
        // Receive a reply from the server
		// NOTE: If you have more data than 149 bytes, it will 
		// 			be received in the next call to "recv"
		// read_size - the length of the received message 
		// 				or an error code in case of failure
		// msg - a buffer where the received message will be stored
		// 149 - the size of the receiving buffer (any more data will be 
		// 		delievered in subsequent "recv" operations
		// 0 - Options and are not necessary here
        if( (read_size = recv(sockfd , srv_reply , 149 , 0)) < 0)
            perror("recv failed");

		// Null termination since we need to print it
		srv_reply[read_size] = '\0';
        printf("Server's reply: %s\n", srv_reply);
    }
  
}

