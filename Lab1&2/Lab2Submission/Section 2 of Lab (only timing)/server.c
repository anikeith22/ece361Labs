#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUF_SIZE 1024

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

    socklen_t client_message_len = sizeof(struct sockaddr_in); // socklen_t basically an integer but you need to input into recvfrom

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

    close(soc_file_descriptor);
    return 0;
}
