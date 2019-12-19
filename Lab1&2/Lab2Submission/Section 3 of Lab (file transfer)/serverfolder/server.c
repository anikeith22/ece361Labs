#include "packet.h"

#define BUF_SIZE 1100

// lab completed by Anikeith bankar (1004073478)
// and Jay Bihola (1004178310)

int print_error (char *msg) {
    perror(msg);
    return (0);
}


int main(int argc, char const *argv[])
{
    // variables of success
    int receive_from_status = 0, bind_status = 0, message_send_status = 0;
    int soc_file_descriptor;
    char *return_msg;
    // if not used properly, read out this message
    if (argc != 2) { // check if there's 2 arguments given, if not ...
        print_error("Error 1:\n Invalid Usage\n Usage Format: server (UDP listen port), i.e., server 2000\n");
    }

    int port = atoi(argv[1]); // atoi converts port argument from char to int
    soc_file_descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // set up socket connection

    // open socket (DGRAM) - dont have to maintain an open connection
    // AF_INET = IPV4
    // IPROTO_UDP = UDP Protocol
    // SOCK_DGRAM = socket type

    if (soc_file_descriptor < 0) {
        print_error("Error 2:\n Socket Connection Error! Unable to establish connection with socket\n");
        return 0;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET; //
    server_address.sin_port = htons(port); // setting up the specified port entered in, also converts port to port to the right format
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(server_address.sin_zero, 0, sizeof(server_address.sin_zero)); //reset

    // bind to socket
    bind_status = (bind(soc_file_descriptor, (struct sockaddr *) &server_address, sizeof(server_address)));
    if (bind_status == -1) {
        print_error("Error 3:\n Socket Binding Error!\n");
    }

    char message_buffer[BUF_SIZE] = {0};

    struct sockaddr_in client_address;

    //socklen_t client_message_len = sizeof(struct sockaddr_in); // socklen_t basically an integer but you need to input into recvfrom
	socklen_t client_message_len = sizeof(struct sockaddr_in);

    // recvfrom the client and store info in client_address so as to send back later
    receive_from_status = recvfrom(soc_file_descriptor, message_buffer, (BUF_SIZE)-1, 0, (struct sockaddr *) &client_address, &client_message_len);

    if (receive_from_status == -1) {
        print_error("Error 4:\n Error Receiving Information! \n");
        return 0;
    }

    // send message back to client based on message received
    if (strcmp(message_buffer, "ftp") == 0) {
        return_msg = "yes";
    } else {
        return_msg = "no";
        return 0;
    }

    message_send_status = sendto(soc_file_descriptor, return_msg, strlen(return_msg), 0, (struct sockaddr *) &client_address, client_message_len);
    //socket, message to send, length of message to send,

    if (message_send_status == -1) {
        print_error("Error 5:\n Return Message Not Sent!\n");
        return 0;
    }

    // Code to receive packets

    // currently received packet
    struct packet * recv_pack;
    int frag_no, total_frag, sizet;
    char * filename, *filedata;

    bool currently_receiving = true;
    FILE * filepointer;

    while (currently_receiving) {
        receive_from_status = recvfrom(soc_file_descriptor,message_buffer,(BUF_SIZE)-1, 0, (struct sockaddr*) &client_address, &client_message_len);

        recv_pack = unpack(message_buffer);
        frag_no = recv_pack->frag_no;
        total_frag = recv_pack->total_frag;
        sizet = recv_pack->size;
        filename = recv_pack->filename;
        filedata = recv_pack->filedata;

        if (frag_no == 1) {
            filepointer = fopen(filename,"w");
            fwrite(filedata, 1, sizet, filepointer);
            currently_receiving = true;

        } else if (frag_no == total_frag) {
            currently_receiving = false;
            fwrite(filedata, 1, sizet, filepointer);
            fclose(filepointer);

        }  else {
           fwrite(filedata, 1, sizet, filepointer);
        }

        free(recv_pack);

        // send acknowledgement
        char *ret_msg2;
        ret_msg2 = "ACK";
        message_send_status = sendto(soc_file_descriptor, ret_msg2, strlen(ret_msg2), 0, (struct sockaddr *) &client_address, client_message_len);
    }

    close(soc_file_descriptor);
    return 0;
}
