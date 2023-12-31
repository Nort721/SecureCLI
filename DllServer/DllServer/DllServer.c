#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "DllServer.h"
#include "Credentials.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_PORT "1234"
#define DEFAULT_BUFLEN 100
#define REGISTERED_USERS_COUNT 2

int main(int argc, char** argv) {
    // load dll file
    HANDLE hFile = NULL;
    DWORD dwLength = NULL;
    DWORD dwBytesRead = 0;
    LPVOID lpBuffer = NULL;

    if (argc < 2)
    {
        ERROR("Wrong usage. Try: DllServer.exe <target dll>");
        return 0;
    }

    char* cpDllFile = argv[1];

    hFile = CreateFileA(cpDllFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        ERROR_WITH_CODE("Failed to open the DLL file");
    dwLength = GetFileSize(hFile, NULL);
    if (dwLength == INVALID_FILE_SIZE || dwLength == 0)
        ERROR_WITH_CODE("Failed to get the DLL file size");
    lpBuffer = HeapAlloc(GetProcessHeap(), 0, dwLength);
    if (!lpBuffer)
        ERROR_WITH_CODE("Failed to alloc a buffer!");
    if (ReadFile(hFile, lpBuffer, dwLength, &dwBytesRead, NULL) == FALSE)
        ERROR_WITH_CODE("Failed to read dll raw data");

    SOCKET ServerSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    // register user accounts
    struct UserCreds registeredUsers[REGISTERED_USERS_COUNT];

    struct UserCreds user1 = { "admin1", "cf3ccb99e6d8ce2ef6f24ad10e05d48b5dd32246c90a5dfde7d264ff1637c03c", "uYyS5zj31o1NDMBd"};
    struct UserCreds user2 = { "admin2", "0b415108ac05bfb8b30b1cd9fac5ca8802a9c4bcdbae2e7c11a31f25d6a74785", "Yl5ch3J6WQ8spJJ9"};

    registeredUsers[0] = user1;
    registeredUsers[1] = user2;

    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ERROR_WITH_CODE("WSAStartup failed");
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    if (getaddrinfo(NULL, DEFAULT_PORT, &hints, &result) != 0) {
        WSACleanup();
        ERROR_WITH_CODE("Resolving server address & port failed");
    }

    // Create a SOCKET for the server to listen for client connections.
    ServerSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ServerSocket == INVALID_SOCKET) {
        freeaddrinfo(result);
        WSACleanup();
        ERROR_WITH_CODE("ServerSocket creation failed");
    }

    // Setup the TCP listening socket
    if (bind(ServerSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        freeaddrinfo(result);
        closesocket(ServerSocket);
        WSACleanup();
        ERROR_WITH_CODE("Bind failed");
    }

    printf("===============================\nDllServer | Services dll memory\n===============================\n\n");
    printf("Listening to incoming requests . . .\n");
    freeaddrinfo(result);

    // Listen for incoming connections
    if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR) {
        // Handle listen error
        closesocket(ServerSocket);
        WSACleanup();
        ERROR_WITH_CODE("Listening failed");
    }

    while (TRUE) {
        // Clear the recvbuf
        memset(recvbuf, 0, sizeof(recvbuf));

        // Accept incoming connection
        ClientSocket = accept(ServerSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            closesocket(ServerSocket);
            WSACleanup();
            ERROR_WITH_CODE("Accept failed");
        }

        if (recv(ClientSocket, recvbuf, recvbuflen, 0) == SOCKET_ERROR) {
            closesocket(ServerSocket);
            WSACleanup();
            ERROR_WITH_CODE("Recv failed");
        }

        char* context = NULL;
        char* username = strtok_s(recvbuf, "|", &context);
        char* recv_pass = strtok_s(NULL, "|", &context);
        int passIndex = 0;

        for (int i = 0; i < REGISTERED_USERS_COUNT; i++) {
            if (strstr(username, registeredUsers[i].username) != NULL) {
                passIndex = i;
                i = REGISTERED_USERS_COUNT;
            }
        }

        char* recv_pass_hashed = hash_password(recv_pass, registeredUsers[passIndex].salt);

        printf("recived_pass: %s pass: %s\n", recv_pass_hashed, registeredUsers[passIndex].password);
        if (strstr(recv_pass_hashed, registeredUsers[passIndex].password) != NULL) {
            printf("Authentication approved, sending dll to %s client.\n", username);
            send(ClientSocket, lpBuffer, dwLength, 0);
        }
        else {
            printf("Authentication failed, sending flagged buffer.\n");

            // Prepare a buffer for the dll
            char* responseBuffer = (char*)malloc(dwLength);
            if (responseBuffer == NULL) {
                closesocket(ClientSocket);
                WSACleanup();
                ERROR_WITH_CODE("memory allocation failed");
            }

            memset(responseBuffer, 0, dwLength);  // Initialize with zeros
            strcpy_s(responseBuffer, dwLength, "AUTH_FAIL");
            send(ClientSocket, responseBuffer, dwLength, 0);
            free(responseBuffer);
        }
    }

    // Clean up
    closesocket(ClientSocket);
    closesocket(ServerSocket);
    WSACleanup();
    HeapFree(GetProcessHeap(), 0, lpBuffer);

    return 0;
}
