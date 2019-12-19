#include "packet.h"

#define DATASIZE 1100

// lab completed by Anikeith bankar (1004073478)
// and Jay Bihola (1004178310)

int print_error (char *msg) {
    perror(msg);
    return (0);
}

int main(int argc, char const *argv[])
{
	// variables for clock and RTT time
	clock_t start_clock, end_clock;
	double rtt_time = 0;
	


    bool check_file = true;
    char input[50], filename[50];
    char const *ip_address = argv[1];
    char const *port = (argv[2]);

    printf("Please enter a filename to transfer in the format: ftp <filename>\n");
    scanf("%s %s", input, filename);


    if(access(filename, F_OK) == -1){
        print_error("file not found");
        return 0;
    }


    int new_socket;
    char message[DATASIZE], *response;
    int message_from_server, get_status;


    struct addrinfo hints;
    struct addrinfo *server_info;
    struct sockaddr_storage server_address; //address info for connector


    memset(&hints, 0, sizeof (hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP; 

    get_status = getaddrinfo(ip_address, port, &hints, &server_info);

    if (get_status < 0) {
        print_error("Failed to get status\n");
    }

    if ((new_socket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol)) < 0) {
        print_error("unable to establish connection with socket\n");
    }



	start_clock = clock();

    if (check_file){
        response = "ftp";
        sendto(new_socket, response, strlen(response), 0, server_info->ai_addr, server_info->ai_addrlen);
    } else {
        response = "nofile";
        sendto(new_socket, response, strlen(response), 0, server_info->ai_addr, server_info->ai_addrlen);
        return 0;
    } 


    socklen_t  addr_len = sizeof(struct sockaddr_storage);

    message_from_server = recvfrom(new_socket, message, DATASIZE-1, 0, (struct sockaddr *)&server_address, &addr_len);
	printf("Message from server: %d\n", message_from_server);
    message[message_from_server] = '\0';

    if(strcmp(message, "yes")==0){
        printf("*******************************************************\n");
        printf("\nResponse from Server: %s\n\n", message);
        printf("A file transfer can start.\n\n");
        printf("*******************************************************\n");
    }
    else{
        perror("Cancelled by Server");
    }

    // create and send packets
    struct packet * head_packet, * current_packet;
    // compressed packet
    char * send_pack;
    int send_pack_length;

    // acknowledgement variables
    char ack_status[4];
    int ack_msg_length;

    printf("filename: %s\n", filename);

    
    // initializing packet structs
    head_packet = segmentize_file(filename);
    current_packet = head_packet;
    printf("TEST5\n");

    while (current_packet != NULL) {
        send_pack = compress_packet(current_packet,&send_pack_length);
		
        sendto(new_socket, send_pack, send_pack_length, 0, server_info->ai_addr, server_info->ai_addrlen);

        // acknowledgement (done using strings)
        message_from_server = recvfrom(new_socket, message, DATASIZE-1, 0, (struct sockaddr *)&server_address, &addr_len);

        if (strcmp(message,"ACK\0") == 0) {
            printf("Status Update\n Packet #: %d Sent Successfully (Acknowledge Received)\n", current_packet->frag_no);
            current_packet = current_packet->next_frag;
            free(send_pack);
            printf("Moved to next packet successfully\n");
        } else {
            printf("Status Update\n Packet #: %d Failed, Retrying in 3\n", current_packet->frag_no);
            printf("2\n");
            printf("1\n");
            printf("Retrying \n");
            continue; 
        }
    }

	end_clock = clock();
	rtt_time = (double) (end_clock-start_clock);
	rtt_time = rtt_time/CLOCKS_PER_SEC;

    // finished transfer
    printf("*******************************************************\n");
    printf("\nTransfer Successfully Completed\n");
	printf("\nTransfer Time (RTT) = %lf\n\n",rtt_time);
    printf("*******************************************************\n");

    freeaddrinfo(server_info);

    // free packets
    current_packet = head_packet;
    struct packet * next = malloc(sizeof(struct packet)); // we can create another packet that we can use to index to 1 packet ahead
    while (current_packet != NULL) {
        next = current_packet->next_frag; // set the temp next pointer to the next fragment so the pointer is not lost
        free(current_packet); // free the current node from memory // no leaks
        current_packet = next; // set the current pointer to the next so it goes in sequence
    }

    // not sure if this line is needed
    
    free(next);
    close(new_socket);
    return 0;

}
