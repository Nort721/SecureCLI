#ifndef CLICNTRL_H
#define CLICNTRL_H

#include <stdio.h>

#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 100

void ReadUsername(char* username);
void ReadPassword(char* password);
void ClearConsoleScreen();

#endif