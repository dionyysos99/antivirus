#ifndef HASHER_H
#define HASHER_H
#define BUFFER_SIZE 4096
#include<stdio.h>
#include<stdlib.h>
#include<openssl/evp.h>
int hash_file(const char *filepath, char *hash_output);
#endif // HASHER_H