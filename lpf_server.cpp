#include <iostream>
#include <winsock2.h>
#include <fstream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

void receiveFile(SOCKET clientSocket, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible de créer le fichier pour l'upload." << std::endl;
        return;
    }

    char buffer[1024];
    int bytesReceived;
    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        file.write(buffer, bytesReceived);
    }
    file.close();
}

void sendFile(SOCKET clientSocket, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier pour le download." << std::endl;
        return;
    }

    char buffer[1024];
    while (file.read(buffer, sizeof(buffer))) {
        send(clientSocket, buffer, file.gcount(), 0);
    }
    send(clientSocket, buffer, file.gcount(), 0);
    file.close();
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 1);

    std::cout << "Serveur en attente de connexion..." << std::endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        char command[256] = {0};
        recv(clientSocket, command, sizeof(command), 0);

        std::string commandStr(command);
        if (commandStr.substr(0, 7) == "upload ") {
            std::string filename = commandStr.substr(7);
            receiveFile(clientSocket, filename);
        } else if (commandStr.substr(0, 9) == "download ") {
            std::string filename = commandStr.substr(9);
            sendFile(clientSocket, filename);
        }

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
