#include "clicntrl.h"
#include <stdio.h>
#include <conio.h>
#include <Windows.h>

void ReadUsername(char* username) {
    if (fgets(username, sizeof(username), stdin)) {
        username[strcspn(username, "\n")] = '\0'; // Remove trailing newline, if any
    }
}

void ReadPassword(char* password) {
    size_t i = 0;
    char c;

    while (i < MAX_PASSWORD_LENGTH - 1) {
        c = _getch(); // Use _getch() from conio.h

        if (c == '\r') { // Check for Enter key
            break;
        }

        password[i++] = c;
        putchar('*'); // Print asterisk for each character
    }

    password[i] = '\0';
}

void ClearConsoleScreen() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = { 0, 0 };
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    // Get the number of character cells in the current buffer
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    // Fill the entire screen with spaces and default attributes
    FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);

    // Move the cursor to the home position
    SetConsoleCursorPosition(hConsole, coordScreen);
}