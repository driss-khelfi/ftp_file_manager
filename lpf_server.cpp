#include <iostream>
#include <winsock2.h>
#include <string>
#include <fstream>
#include <direct.h>  // Pour _mkdir
#include <windows.h> // Pour DeleteFile

#pragma comment(lib, "ws2_32.lib")

void receiveFile(SOCKET clientSocket, const std::string& user, const std::string& filename) {
    std::string userDir = "./" + user; // Création du chemin du dossier utilisateur
    _mkdir(userDir.c_str()); // Crée le dossier si il n'existe pas, ignore l'erreur si le dossier existe déjà

    std::ofstream file(userDir + "/" + filename, std::ios::binary);
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

void sendFile(SOCKET clientSocket, const std::string& user, const std::string& filename) {
    std::ifstream file("./" + user + "/" + filename, std::ios::binary);
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

void deleteFile(SOCKET clientSocket, const std::string& user, const std::string& filename) {
    std::string filePath = "./" + user + "/" + filename;
    if (DeleteFile(filePath.c_str()) == 0) {
        std::cerr << "Erreur : Impossible de supprimer le fichier." << std::endl;
    }
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
        size_t spacePos1 = commandStr.find(' ');
        size_t spacePos2 = commandStr.find(' ', spacePos1 + 1);

        if (spacePos1 == std::string::npos || spacePos2 == std::string::npos) {
            std::cerr << "Commande mal formée." << std::endl;
            closesocket(clientSocket);
            continue;
        }

        std::string user = commandStr.substr(0, spacePos1);
        std::string action = commandStr.substr(spacePos1 + 1, spacePos2 - spacePos1 - 1);
        std::string filename = commandStr.substr(spacePos2 + 1);

        if (action == "upload") {
            receiveFile(clientSocket, user, filename);
        } else if (action == "download") {
            sendFile(clientSocket, user, filename);
        } else if (action == "delete") {
            deleteFile(clientSocket, user, filename);
        } else {
            std::cerr << "Action non reconnue. Utilisez upload, download ou delete." << std::endl;
        }

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
