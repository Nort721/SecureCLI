#ifndef CREDENTIALS_H
#define CREDENTIALS_H

#define HASH_ITERATIONS 5

char* hash_password(const char* password, const char* salt);

struct UserCreds {
	char* username;
	char* password;
	char* salt;
};

#endif