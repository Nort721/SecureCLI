#include "Credentials.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

char* hash_password(const char* password, const char* salt, int iterations) {
    // Concatenate the password and salt
    char* combined = (char*)malloc(strlen(password) + strlen(salt) + 1);
    if (!combined) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    strcpy(combined, password);
    strcat(combined, salt);

    // Perform multiple iterations of SHA-256 hashing
    for (int i = 0; i < iterations; i++) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, combined, strlen(combined));
        SHA256_Final(hash, &sha256);

        // Convert the hash to hexadecimal string representation
        char* temp_combined = (char*)malloc(2 * SHA256_DIGEST_LENGTH + 1);
        if (!temp_combined) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < SHA256_DIGEST_LENGTH; j++) {
            sprintf(&temp_combined[j * 2], "%02x", hash[j]);
        }

        // Update the combined string with the new hash
        strcpy(combined, temp_combined);
        free(temp_combined);
    }

    free(combined);
    return combined; // Return the final hash as a hexadecimal string
}