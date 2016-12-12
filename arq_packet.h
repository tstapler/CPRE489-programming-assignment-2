#ifndef ARQPKT
#define ARQPKT
typedef struct {
	char number[2];
	char data[2];
} packet;

typedef struct {
	char number[2];
} ack;

#define DONE 0
#define NOT_DONE 1 
#define SS 0
#define CA 1
#define FR 2

void send_ack(int sockfd, int sequence_number);

void send_packet(int sockfd, packet *pkt);

int receive_packet(int sockfd, packet *pkt, int *read_size);
int receive_ack(int sockfd);

void fill_packet(packet *pkt, unsigned int number, char data1, char data2);

void initialze_packet_buff(packet *window_buff, char *message);
#endif

