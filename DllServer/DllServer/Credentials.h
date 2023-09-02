#ifndef CREDENTIALS_H
#define CREDENTIALS_H

char* hash_password(const char* password, const char* salt, int iterations);

struct UserCreds {
	char* username;
	char* password;
	char* salt;
};

#endif