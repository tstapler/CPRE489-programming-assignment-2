#include<stdio.h>
#include<sys/socket.h>
#include <string.h> 

void secondary(int client_sock) {
    int read_size;
	char msg[150];

    // Receive a message from client
	// read_size - the length of the received message 
	// 				or an error code in case of failure
	// msg - a buffer where the received message will be stored
	// 149 - the size of the receiving buffer (any more data will be 
	// 		delievered in subsequent "recv" operations
	// 0 - Options and are not necessary here
    while( (read_size = recv(client_sock , msg , 149 , 0)) > 0 )
    {
		// Null termination since we will use strlen(msg) later on
		// and we need to print it now
		msg[read_size] = '\0';
		printf("%s\n", msg);
        // Send the message back to client
		// msg - the message to be sent
		// strlen(msg) - length of the message
		// 0 - Options and are not necessary here
		// If return value < 0, an error occured
        if (send(client_sock , msg , strlen(msg) , 0) < 0)
        	perror("Send failed");
    }
     
    // Might move back to receiver
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

}


