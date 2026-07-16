#include "../include/hash_table.h"
static unsigned int hash_to_bucket_index(const char *hash_hex)
{
    unsigned int index = 0;

    for (int i = 0; i < 8 && hash_hex[i] != '\0'; i++)
    {
        int val = hex_char_to_val(hash_hex[i]);
        if (val == -1)
            continue;  // geçersiz karakter varsa atla (savunmacı programlama)

        index = (index * 16) + (unsigned int)val;
    }

    return index % HASH_TABLE_SIZE;
}
int db_insert(signature_db_t *db, const char *hash_hex, const char *malware_name)
{
    signature_entry_t *new_entry = (signature_entry_t *)malloc(sizeof(signature_entry_t));
    if (!new_entry)
        return -1;  

    strncpy(new_entry->hash_hex, hash_hex, SHA256_HEX_LEN);
    new_entry->hash_hex[SHA256_HEX_LEN - 1] = '\0';  // null-terminate
    strncpy(new_entry->malware_name, malware_name, sizeof(new_entry->malware_name));
    new_entry->malware_name[sizeof(new_entry->malware_name) - 1] = '\0';  // null-terminate
    
    unsigned int index = hash_to_bucket_index(hash_hex);
    new_entry->next = db->buckets[index];
    db->buckets[index] = new_entry;

    return 0;  
}
signature_entry_t *db_lookup(signature_db_t *db, const char *hash_hex)
{
    unsigned int index = hash_to_bucket_index(hash_hex);
    signature_entry_t *current = db->buckets[index];

    while (current)
    {
        if (strncmp(current->hash_hex, hash_hex, SHA256_HEX_LEN) == 0)
            return current;  
        current = current->next;
    }

    return NULL;  
}
int db_load_from_file(signature_db_t *db, const char *filepath)
{
    FILE *file = fopen(filepath, "r");
    if (!file)
      {
        perror("fopen (db_load_from_file)");
        return -1;  
      }  

    char line[256];
    int loaded_count = 0;
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = 0;
        if(strlen(line) == 0)
            continue;
        char *comma = strchr(line, ',');
        if (!comma)
            {
                fprintf(stderr,"Geçersiz format: %s\n", line);
                continue;
            }
        *comma = '\0';
        const char *hash_part = line;
        const char *name_part = comma + 1;
        if(db_insert(db, hash_part, name_part) != 0)
            {
                fprintf(stderr, "İmza eklenemedi: %s\n", line);
                continue;
            }
        loaded_count++;
    }

    fclose(file);
    printf("Toplam %d imza yüklendi.\n", loaded_count);
    return 0;  
}
void db_free(signature_db_t *db)
{
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        signature_entry_t *current = db->buckets[i];
        while (current)
        {
            signature_entry_t *temp = current;
            current = current->next;
            free(temp);
        }
        db->buckets[i] = NULL;
    }
}
void db_init(signature_db_t *db) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        db->buckets[i] = NULL;
    }
}
