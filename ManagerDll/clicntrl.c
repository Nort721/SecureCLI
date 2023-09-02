#include <stdio.h>
#include <conio.h>

void read_password(char* password, size_t max_length) {
    size_t i = 0;
    char c;

    while (i < max_length - 1) {
        c = _getch(); // Use _getch() from conio.h

        if (c == '\r') { // Check for Enter key
            break;
        }

        password[i++] = c;
        putchar('*'); // Print asterisk for each character
    }

    password[i] = '\0';
}

