#include "../include/aho_corasick_search.h"
static void report_match(match_list_t *results, trie_node_t *node, long offset)
{
    if (node->is_end_of_pattern && results->match_count < MAX_MATCHES)
    {
        strncpy(results->matches[results->match_count].pattern_name, node->pattern_name, 127);
        results->matches[results->match_count].pattern_name[127] = '\0';
        results->matches[results->match_count].offset = offset;
        results->match_count++;
    }
}
int aho_corasick_search(trie_node_t *root, const char *filepath, match_list_t *results)
{
    FILE *file = fopen(filepath, "rb");
    if (!file)
    {
        fprintf(stderr, "Dosya acilamadi: %s\n", filepath);
        return -1;
    }

    results->match_count = 0;
    trie_node_t *current = root;
    long offset = 0;
    
    unsigned char buffer[4096];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        for (size_t i = 0; i < bytes_read; i++)
        {
            unsigned char byte = buffer[i];
            while(current !=root && current->children[byte] == NULL)
            {
                current = current->fail;
            }
            if(current->children[byte] != NULL)
            {
                current = current->children[byte];
            }
            trie_node_t *temp = current;
            while(temp != root && temp!=NULL)
            {
                report_match(results, temp, offset);
                temp = temp->fail;
            }
            offset++;
        }
        if(ferror(file))
        {
            fprintf(stderr, "Dosya okuma hatasi: %s\n", filepath);
            fclose(file);
            return -1;
        }
    }
    fclose(file);
    return 0;
}