#ifndef AHO_CORASİCK_SEARCH_H
#define AHO_CORASİCK_SEARCH_H
#define MAX_MATCHES 1024
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/trie.h"

typedef struct
{
    char pattern_name[128];
    long offset;
} match_result_t;
typedef struct
{
    match_result_t matches[MAX_MATCHES];
    int match_count;
}match_list_t;
int aho_corasick_search(trie_node_t *root, const char *filepath, match_list_t *results);
#endif // AHO_CORASİCK_SEARCH_H