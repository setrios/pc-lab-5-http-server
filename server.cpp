#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

void handleClient(int clientSocket) {
    const char* response = "HTTP/1.1 200 OK\r\n\r\nHello from server";
    send(clientSocket, response, strlen(response), 0);
    close(clientSocket);
}

int main() {
    int serverSocket;
    struct sockaddr_in serverAddr;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Bind failed" << std::endl;
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) == -1)
    {
        std::cerr << "Listen failed" << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Server listening on port " << PORT << "..." << std::endl;
    std::cout << "Visit: http://localhost:8080" << std::endl;

    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

        if (clientSocket == -1) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        std::cout << "Connection from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;

        handleClient(clientSocket);
    }

    close(serverSocket);
    return 0;
}
