#include "crypt.h"

int hash_memory(int hash, unsigned char *data, int len, unsigned char *dst)
{
    union hash_state md;

    if (hash == -1) { crypt_error = "Invalid hash passed to hash_memory()."; return CRYPT_ERROR; }

    hash_descriptor[hash].init(&md);
    hash_descriptor[hash].process(&md, data, len);
    hash_descriptor[hash].done(&md, dst);
    memset(&md, 0, sizeof(md));
    return CRYPT_OK;
}

int hash_file(int hash, char *fname, unsigned char *dst)
{
    union hash_state md;
    FILE *in;
    unsigned char buf[512];
    int x;

    if (hash == -1) { crypt_error = "Invalid hash passed to hash_memory()."; return CRYPT_ERROR; }
    in = fopen(fname, "rb");
    if (in == NULL) { crypt_error = "Error opening file in hash_memory()."; return CRYPT_ERROR; }
    hash_descriptor[hash].init(&md);
    do {
        x = fread(buf, 1, 512, in);
        hash_descriptor[hash].process(&md, buf, x);
    } while (x == 512);
    fclose(in);
    hash_descriptor[hash].done(&md, dst);
    memset(&md, 0, sizeof(md));
    return CRYPT_OK;
}


