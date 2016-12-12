#ifndef ARQPKT
#define ARQPKT
typedef struct {
	char type;
	char number;
	char data[2];
	char crc[2];
} packet;

#define DONE 0
#define NOT_DONE 1 
#define DATA 1
#define ACK 2
#define NAK 3

#define WINDOW_SIZE 3


void send_packet(int sockfd, packet *pkt);

int receive_packet(int sockfd, packet *pkt);

void fill_packet(packet *pkt, int type, int number, char data1, char data2);

void initialze_packet_buff(packet *window_buff, char *message);
#endif

