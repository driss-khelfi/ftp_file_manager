#include <iostream>
#include <winsock2.h>
#include <string>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")

void uploadFile(SOCKET clientSocket, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier pour l'upload." << std::endl;
        return;
    }

    char buffer[1024];
    while (file.read(buffer, sizeof(buffer))) {
        send(clientSocket, buffer, file.gcount(), 0);
    }
    send(clientSocket, buffer, file.gcount(), 0);
    file.close();
}

void downloadFile(SOCKET clientSocket, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible de créer le fichier pour le download." << std::endl;
        return;
    }

    char buffer[1024];
    int bytesReceived;
    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        file.write(buffer, bytesReceived);
    }
    file.close();
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: ./lpf user@ip:port -upload|-download|-delete filename" << std::endl;
        return 1;
    }

    std::string userIpPort = argv[1];
    std::string command = argv[2];
    std::string filename = argv[3];

    size_t atPos = userIpPort.find('@');
    size_t colonPos = userIpPort.find(':');
    std::string user = userIpPort.substr(0, atPos);
    std::string ip = userIpPort.substr(atPos + 1, colonPos - atPos - 1);
    int port = std::stoi(userIpPort.substr(colonPos + 1));

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Erreur : Connexion au serveur échouée." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::string commandStr;
    if (command == "-upload") {
        commandStr = user + " upload " + filename;
        send(clientSocket, commandStr.c_str(), commandStr.size() + 1, 0);
        uploadFile(clientSocket, filename);
    } else if (command == "-download") {
        commandStr = user + " download " + filename;
        send(clientSocket, commandStr.c_str(), commandStr.size() + 1, 0);
        downloadFile(clientSocket, filename);
    } else if (command == "-delete") {
        commandStr = user + " delete " + filename;
        send(clientSocket, commandStr.c_str(), commandStr.size() + 1, 0);
    } else {
        std::cerr << "Commande non reconnue. Utilisez -upload, -download, ou -delete." << std::endl;
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
