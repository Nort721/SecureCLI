#ifndef NET_H
#define NET_H

#include <string.h>
#include <stdbool.h>

#define ERROR_WITH_CODE(e) (printf("[-] ERROR: %s, 0x%x\r\n", e, GetLastError()))

bool AquireAndLoadDll(char* credentials);

#endif