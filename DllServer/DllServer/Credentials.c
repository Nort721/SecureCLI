#include "Credentials.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

char* hash_password(const char* password, const char* salt) {
    EVP_MD_CTX* mdctx;
    const EVP_MD* md;
    char* result = NULL;
    unsigned char* digest = NULL;
    size_t digest_len;
    size_t result_len = 2 * EVP_MAX_MD_SIZE + 1; // Maximum size for SHA-256 hash

    md = EVP_get_digestbyname("sha256");
    if (!md) {
        fprintf(stderr, "Error: SHA-256 not supported.\n");
        return NULL;
    }

    mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        fprintf(stderr, "Error: Unable to create MD context.\n");
        return NULL;
    }

    EVP_MD_CTX_init(mdctx);

    for (int i = 0; i < HASH_ITERATIONS; i++) {
        if (i == 0) {
            EVP_DigestInit_ex(mdctx, md, NULL);
            EVP_DigestUpdate(mdctx, password, strnlen_s(password, 255));
            EVP_DigestUpdate(mdctx, salt, strnlen_s(salt, 255));
        }
        else {
            EVP_DigestInit_ex(mdctx, md, NULL);
            EVP_DigestUpdate(mdctx, digest, digest_len);
            EVP_DigestUpdate(mdctx, salt, strnlen_s(salt, 255));
        }

        digest_len = EVP_MD_size(md);
        digest = (unsigned char*)malloc(digest_len);

        if (!digest) {
            fprintf(stderr, "Error: Memory allocation failed.\n");
            return NULL;
        }

        EVP_DigestFinal_ex(mdctx, digest, &digest_len);
    }

    result = (char*)malloc(result_len);

    if (!result) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return NULL;
    }

    result[0] = '\0'; // Ensure the result is initially empty

    for (size_t i = 0; i < digest_len; i++) {
        snprintf(result + i * 2, result_len - i * 2, "%02x", digest[i]);
    }

    EVP_MD_CTX_free(mdctx);
    free(digest);

    return result;
}