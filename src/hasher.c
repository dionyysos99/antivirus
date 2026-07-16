/*İncludes*/
#include "../include/hasher.h"
/*-----------------------*/
int hash_file(const char *filename, char *hash_output)
{
    size_t  returned_value;
    unsigned char   hash[EVP_MAX_MD_SIZE];
    unsigned int    hash_len;    
    unsigned char   buffer[BUFFER_SIZE];
    FILE *fp = fopen(filename, "rb");
    if(!fp)
    {
        perror("fopen");
        return -1;
    }
    EVP_MD_CTX *ctx= EVP_MD_CTX_new();
    if(!ctx)
    {
        fprintf(stderr,"EVP_MD_CTX_new() hatası oluştu.\n");
        fclose(fp);
        return -1;
    }
    if(EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1)
    {
        fprintf(stderr,"EVP_DigestInit_ex() hatası oluştu.\n");
        EVP_MD_CTX_free(ctx);
        fclose(fp);
        return -1;
    }
    while((returned_value = fread(buffer, 1, BUFFER_SIZE, fp)) > 0)
    {
        if(EVP_DigestUpdate(ctx, buffer, returned_value) != 1)
        {
            fprintf(stderr,"EVP_DigestUpdate() hatası oluştu.\n");
            EVP_MD_CTX_free(ctx);
            fclose(fp);
            return -1;
        }
    }
    if(ferror(fp))
    {
        fprintf(stderr,"Dosya okuma hatası oluştu.\n");
        fclose(fp);
        return -1;
    }  
    if(EVP_DigestFinal_ex(ctx, hash, &hash_len) != 1)
    {
        fprintf(stderr,"EVP_DigestFinal_ex() hatası oluştu.\n");
        EVP_MD_CTX_free(ctx);
        fclose(fp);
        return -1;
    }
    for (unsigned int i = 0; i < hash_len; i++)
    {
        sprintf(hash_output + (i * 2), "%02x", hash[i]);
    }
    hash_output[hash_len * 2] = '\0';

    EVP_MD_CTX_free(ctx);
    fclose(fp);
    return 0;
}
