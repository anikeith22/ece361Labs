
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>     //includes sockaddr
#include <netdb.h>          //includes addrinfo
#include <arpa/inet.h>      //includes inet_ntop etc
#include <unistd.h>         //includes close()
#include <errno.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

	
#define cID_len 30 


struct message {
	unsigned int type; 				// type of the message being sent 
	unsigned int size; 				// size of length of message  
	char source[cID_len]; // ID of the client sending the message 
	char data[MAX_DATA]; 	// Data within the packet 
}; 

void  decode_message(struct message * message, char * destination) {
	int iterator = 0;
	// make sure buffer is clean 
	memset(destination, 0, sizeof(char)*2048);  

	// we will use "~" to differentiate attributes within the message 
	
	// what type of message is being sent 
	sprintf(destination, "%d", message->type); 
	iterator = strlen(destination); 
	memcpy(destination + iterator++, "~", sizeof(char));  

	// what is the size of packet message  
	sprintf(iterator + destination, "%d", message->size);
	iterator = strlen(destination);
	memcpy(destination + iterator++, "~", sizeof(char));

	// copy in the client 
	sprintf(destination + iterator, "%s", message->source);
	iterator = strlen(destination);
	memcpy(destination+iterator++, "~", sizeof(char));

	// copy in the actual message of the packet
	sprintf(destination + iterator, "%s", message->data);
} 

struct message * string_to_message(const char * source) {
	struct message * new_message = (struct message *)malloc(sizeof(struct message));
    char buffer[3000]; 
    strcpy(buffer, source); 
    sscanf(buffer,"%d~%d~%[^~]~%[^~]",&new_message->type, &new_message->size, new_message->source, new_message->data); 
    return new_message; 
}
void print_message(const struct message * message) {
	printf("\nType: %d \nMessage Size: %d \nID: %s \nData of Message: %s", message->type, message->size, message->source, message->data);
}






