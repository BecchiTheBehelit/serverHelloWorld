#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(void) {
    int socket_desc;
    struct sockaddr_in server_addr, client_addr;
    char client_message[2000];
    char server_message[] = "Hello from AWS server!";
    socklen_t client_struct_length = sizeof(client_addr);

    // Clean buffers:
    memset(client_message, '\0', sizeof(client_message));

    // Create UDP socket:
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(socket_desc < 0){
        printf("Error while creating socket\n");
        return -1;
    }

    printf("Socket created successfully\n");

    // Set IP and port:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = 0;  // Let the system assign any available port
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind to the set IP and port:
    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        printf("Couldn't bind to the port\n");
        return -1;
    }

    // Get the assigned port number
    socklen_t len = sizeof(server_addr);
    if (getsockname(socket_desc, (struct sockaddr *)&server_addr, &len) == -1) {
        perror("getsockname");
        return -1;
    }

    printf("Listening on port %d\n", ntohs(server_addr.sin_port));

    while(1) {
        // Receive client's message:
        if (recvfrom(socket_desc, client_message, sizeof(client_message), 0,
                     (struct sockaddr*)&client_addr, &client_struct_length) < 0){
            printf("Couldn't receive\n");
            continue;
        }
        printf("Received message from IP: %s and port: %i\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        printf("Msg from client: %s\n", client_message);

        // Respond to client:
        if (sendto(socket_desc, server_message, strlen(server_message), 0,
                   (struct sockaddr*)&client_addr, client_struct_length) < 0){
            printf("Can't send\n");
            continue;
        }
        
        // Clear the client_message buffer for the next message
        memset(client_message, '\0', sizeof(client_message));
    }
    
    // Close the socket:
    close(socket_desc);
    return 0;
}