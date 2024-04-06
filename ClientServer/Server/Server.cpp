#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h> 
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

SOCKET ClientSocket;

struct Coordinate {
    int x;
    int y;
};

vector<Coordinate> walls = {
    {4, 1}, {4, 2}, {4, 3},
    {6, 1}, {6, 2}, {7, 2},
    
};

bool isWall(int x, int y) {
    for (const auto& wall : walls) {
        if (wall.x == x && wall.y == y) {
            return true;
        }
    }
    return false;
}

void moveSmiley(int direction) {
    char message[2];
    message[0] = '0' + direction;
    message[1] = '\0';
    send(ClientSocket, message, 2, 0);
}

void showServerSmileyCoordinates() {
    cout << "Server Smiley Coordinates: 11\n";
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup failed\n";
        return 1;
    }

    struct addrinfo* result = NULL, * ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, "27015", &hints, &result) != 0) {
        cout << "getaddrinfo failed\n";
        WSACleanup();
        return 1;
    }

    SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    if (bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        cout << "bind failed\n";
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        cout << "listen failed\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    cout << "Waiting for client connection...\n";

    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        cout << "accept failed\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    closesocket(ListenSocket);

    cout << "Client connected\n";

    while (true) {
        int direction;
        char buffer[2];
        int recvResult = recv(ClientSocket, buffer, 2, 0);
        if (recvResult > 0) {
            direction = buffer[0] - '0';
            switch (direction) {
            case 1:
                if (!isWall(11 - 1, 11)) 
                    cout << "Smiley moved left\n";
                break;
            case 2:
                if (!isWall(11 + 1, 11)) 
                    cout << "Smiley moved right\n";
                break;
            case 3:
                if (!isWall(11, 11 - 1)) 
                    cout << "Smiley moved up\n";
                break;
            case 4:
                if (!isWall(11, 11 + 1)) 
                    cout << "Smiley moved down\n";
                break;
            default:
                break;
            }
        }
        else if (recvResult == 0) {
            cout << "Client disconnected\n";
            break;
        }
        else {
            cout << "recv failed\n";
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    }

    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}


