#include <iostream>
#include <conio.h> 
#include <winsock2.h>
#include <ws2tcpip.h> 

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

SOCKET ConnectSocket;

void moveSmiley(int direction) {
    char message[2];
    message[0] = '0' + direction;
    message[1] = '\0';
    send(ConnectSocket, message, 2, 0);
}

void showClientSmileyCoordinates() {
    cout << "Client Smiley Coordinates: 13\n";
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup failed\n";
        return 1;
    }

    struct addrinfo* result = NULL, * ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo("localhost", "27015", &hints, &result) != 0) {
        cout << "getaddrinfo failed\n";
        WSACleanup();
        return 1;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            cout << "Error at socket(): " << WSAGetLastError() << endl;
            freeaddrinfo(result);
            WSACleanup();
            return 1;
        }

        if (connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }

        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Unable to connect to server\n";
        WSACleanup();
        return 1;
    }

    printf("Connected to server\n");

    while (true) {
        int key = _getch();
        switch (key) {
        case 72: // Up 
            moveSmiley(3);
            break;
        case 80: // Down 
            moveSmiley(4);
            break;
        case 75: // Left 
            moveSmiley(1);
            break;
        case 77: // Right 
            moveSmiley(2);
            break;
        default:
            break;
        }
    }

    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}



