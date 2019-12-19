#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h> // includes addrinfo
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

// lab completed by Anikeith bankar (1004073478)
// and Jay Bihola (1004178310)

int print_error (char *msg) {
    perror(msg);
    return (0);
}

int main(int argc, char const *argv[])
{
    // clock variables for start and end
    clock_t start_clock, end_clock;
    // clock difference variable
    double rtt_time = 0;

    bool check_file = true;
    char input[100], filename[100];
    char const *ip_address = argv[1];
    char const *port = (argv[2]);

    printf("Please enter a filename to transfer in the format: ftp <filename>\n");
    scanf("%s %s", input, filename);


    if(access(filename, F_OK) == -1){
        print_error("file not found");
        check_file = false;
    }

    int new_socket;
    char message[100], *response;
    int message_from_server, get_status;


    struct addrinfo hints;
    struct addrinfo *server_info;
    struct sockaddr_storage server_address; //address info for connector

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

    // get the current time
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

    message_from_server = recvfrom(new_socket, message, 99, 0, (struct sockaddr *)&server_address, &addr_len);

    message[message_from_server] = '\0';

    // get the current time
    end_clock = clock();

    // calculate time difference to find round trip time
    rtt_time = (double) (end_clock - start_clock);
    rtt_time = (rtt_time)/CLOCKS_PER_SEC;


    if(strcmp(message, "yes")==0){
        printf("*******************************************************\n");
        printf("\nResponse from Server: %s\n\n", message);
        printf("A file transfer can start.\n\n");
        printf("Measured round trip time: %f \n\n", rtt_time);
        printf("*******************************************************\n");
    }
    else{
        perror("Cancelled by Server");
        printf("Measured round trip time: %f \n", rtt_time);
    }

    // report round trip time

    freeaddrinfo(server_info);
    close(new_socket);
    return 0;

}
