#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "Net.h"
#include "CliCntrl.h"
#include "ManualMapper.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_PORT "1234"

bool AquireAndLoadDll(char* credentials) {
	HANDLE hTargetProcess = NULL;
	HANDLE hModule = NULL;
	DWORD dwBytesRead = 0;
	DWORD dwTargetProcessID = GetCurrentProcessId();

	struct addrinfo* result = NULL, hints;

	// Initialize Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		ERROR_WITH_CODE("initialization failed");
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	if (getaddrinfo(NULL, DEFAULT_PORT, &hints, &result) != 0) {
		WSACleanup();
		ERROR_WITH_CODE("getaddrinfo failed");
	}

	// Create socket
	SOCKET clientSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (clientSocket == INVALID_SOCKET) {
		WSACleanup();
		ERROR_WITH_CODE("socket creation failed");
	}

	// Connect to the server
	if (connect(clientSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
		closesocket(clientSocket);
		WSACleanup();
		ERROR_WITH_CODE("connection failed");
	}

	// Send the credentials
	if (send(clientSocket, credentials, (int)strlen(credentials), 0) == SOCKET_ERROR) {
		closesocket(clientSocket);
		WSACleanup();
		ERROR_WITH_CODE("send failed");
	}

	// Prepare a buffer for the dll
	int maxBufferSize = 16384; // Adjust according to your DLL size
	char* receivedBuffer = (char*)malloc(maxBufferSize);
	if (receivedBuffer == NULL) {
		closesocket(clientSocket);
		WSACleanup();
		ERROR_WITH_CODE("memory allocation failed");
	}

	int bytesReceivedCount = recv(clientSocket, receivedBuffer, maxBufferSize, 0);
	if (bytesReceivedCount == SOCKET_ERROR) {
		// Handle receive error
		closesocket(clientSocket);
		WSACleanup();
		ERROR_WITH_CODE("recv failed");
	}

	// shutdown the connection since no more data will be sent
	if (shutdown(clientSocket, SD_SEND) == SOCKET_ERROR) {
		closesocket(clientSocket);
		WSACleanup();
		ERROR_WITH_CODE("shutdown failed");
	}

	// Clean up
	closesocket(clientSocket);
	WSACleanup();
	
	// Check for the authentication flag
	if (strncmp(receivedBuffer, "AUTH_FAIL", strlen("AUTH_FAIL")) == 0) {
		return false;
	}

	// Create an LPVOID variable and copy the received data
	LPVOID lpBuffer = malloc(bytesReceivedCount);
	if (lpBuffer == NULL) {
		ERROR_WITH_CODE("memory allocation failed");
	}
	memcpy(lpBuffer, receivedBuffer, bytesReceivedCount);

	hTargetProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
		PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, dwTargetProcessID);
	if (!hTargetProcess)
		ERROR_WITH_CODE("Failed to open the target process");

	hModule = LoadRemoteLibraryR(hTargetProcess, lpBuffer, bytesReceivedCount, NULL);
	if (!hModule)
		ERROR_WITH_CODE("Failed to inject the DLL");

	ClearConsoleScreen();
	printf("\n[+] Login successful.\n");
	printf("[+] Injected the DLL into process %d.", dwTargetProcessID);

	free(lpBuffer);
	free(receivedBuffer);

	WaitForSingleObject(hModule, -1);
	return true;
}