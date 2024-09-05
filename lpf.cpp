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
        std::cerr << "Usage: ./lpf ip:port -upload|-download filename" << std::endl;
        return 1;
    }

    std::string ipPort = argv[1];
    std::string command = argv[2];
    std::string filename = argv[3];

    size_t colonPos = ipPort.find(':');
    std::string ip = ipPort.substr(0, colonPos);
    int port = std::stoi(ipPort.substr(colonPos + 1));

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

    if (command == "-upload") {
        std::string uploadCommand = "upload " + filename;
        send(clientSocket, uploadCommand.c_str(), uploadCommand.size() + 1, 0);
        uploadFile(clientSocket, filename);
    } else if (command == "-download") {
        std::string downloadCommand = "download " + filename;
        send(clientSocket, downloadCommand.c_str(), downloadCommand.size() + 1, 0);
        downloadFile(clientSocket, filename);
    } else {
        std::cerr << "Commande non reconnue. Utilisez -upload ou -download." << std::endl;
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
