#ifndef HASH_TABLE_H
#define HASH_TABLE_H
#define HASH_TABLE_SIZE 1024
#define SHA256_HEX_LEN 65  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
typedef struct signature_entry
{
    char hash_hex[SHA256_HEX_LEN];
    char malware_name[128];
    struct signature_entry *next;
} signature_entry_t;
typedef struct
{
    signature_entry_t *buckets[HASH_TABLE_SIZE];
} signature_db_t;
void db_init(signature_db_t *db);
void db_free(signature_db_t *db);
int db_insert(signature_db_t *db, const char *hash_hex, const char *malware_name);
int db_load_from_file(signature_db_t *db, const char *filepath);
signature_entry_t *db_lookup(signature_db_t *db, const char *hash_hex);
#endif // HASH_TABLE_H