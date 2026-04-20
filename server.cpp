#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <fstream>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 4096
#define WWW_DIR "www"

std::string parseRequestPath(const std::string& request) {
    std::istringstream stream(request);
    std::string method, path, version;

    stream >> method >> path >> version;

    // log parsed request
    std::cout << "Method: " << method << ", Path: " << path << ", Version: " << version << std::endl;

    return path;
}

std::string mapPathToFile(const std::string& path) {
    std::string filePath = WWW_DIR;

    // root path maps to index.html
    if (path == "/")
        filePath += "/index.html";
    else
        filePath += path;

    return filePath;
}

bool readFile(const std::string& filePath, std::string& content) {
    std::ifstream file(filePath);

    if (!file.is_open())
        return false;

    std::stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    file.close();

    return true;
}

std::string buildHttpResponse(int statusCode, const std::string& content) {
    std::string response;

    // status line
    if (statusCode == 200)
        response = "HTTP/1.1 200 OK\r\n";
    else if (statusCode == 404)
        response = "HTTP/1.1 404 Not Found\r\n";

    // headers
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + std::to_string(content.length()) + "\r\n";
    response += "\r\n";

    // body
    response += content;

    return response;
}

void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    // read http request
    int bytesRead = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
    if (bytesRead <= 0) {
        close(clientSocket);
        return;
    }

    std::string request(buffer);
    std::string path = parseRequestPath(request);

    // map url path to file
    std::string filePath = mapPathToFile(path);
    std::string content;

    // try to read file
    if (readFile(filePath, content)) {
        std::cout << "File found: " << filePath << std::endl;
        std::string response = buildHttpResponse(200, content);
        send(clientSocket, response.c_str(), response.length(), 0);
    }
    else {
        std::cout << "File not found: " << filePath << std::endl;
        std::string notFoundContent = "<html><body><h1>404 Not Found</h1></body></html>";
        std::string response = buildHttpResponse(404, notFoundContent);
        send(clientSocket, response.c_str(), response.length(), 0);
    }

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

        // create new thread for each client
        std::thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    close(serverSocket);
    return 0;
}
