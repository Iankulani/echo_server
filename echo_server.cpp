#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

void print_error_and_exit(const string& message) {
    cerr << message << endl;
    exit(EXIT_FAILURE);
}

int main() {
    string server_ip;
    int server_port;
    int sockfd, new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[1024];

    // Prompt user for server details
    cout << "Enter the server IP address:";
    cin >> server_ip;
    cout << "Enter the server port number:";
    cin >> server_port;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        print_error_and_exit("Error creating socket!");
    }

    // Set up the server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());
    memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        print_error_and_exit("Bind failed!");
    }

    // Listen for incoming connections
    if (listen(sockfd, 10) < 0) {
        print_error_and_exit("Listen failed!");
    }

    cout << "Echo server listening on " << server_ip << ":" << server_port << "..." << endl;

    addr_size = sizeof(client_addr);

    // Accept incoming client connection
    new_sock = accept(sockfd, (struct sockaddr*)&client_addr, &addr_size);
    if (new_sock < 0) {
        print_error_and_exit("Accept failed!");
    }

    cout << "Connection established with client!" << endl;

    // Loop to handle communication with the client
    while (true) {
        // Clear the buffer
        memset(buffer, 0, sizeof(buffer));

        // Receive message from the client
        ssize_t bytes_received = recv(new_sock, buffer, sizeof(buffer), 0);
        if (bytes_received < 0) {
            print_error_and_exit("Error receiving data from client!");
        }
        else if (bytes_received == 0) {
            cout << "Client disconnected." << endl;
            break;
        }

        // Echo the message back to the client
        cout << "Received message: " << buffer << endl;
        if (send(new_sock, buffer, bytes_received, 0) < 0) {
            print_error_and_exit("Error sending data to client!");
        }
        cout << "Message sent back to client!" << endl;
    }

    // Close the connection
    close(new_sock);
    close(sockfd);
    cout << "Server shut down." << endl;

    return 0;
}
