#ifndef TRIE_H
#define TRIE_H
#define ALPHABET_SIZE 256
#define MAX_PATTERN_LEN 256
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/hash_table.h"

typedef struct trie_node {
    struct trie_node *children[ALPHABET_SIZE];
    struct trie_node *fail;
    int is_end_of_pattern;
    char pattern_name[128];
    int output_count;
} trie_node_t;
typedef struct queue_node
{
    trie_node_t *trie_node;
    struct queue_node *next;
} queue_node_t;

typedef struct
{
    queue_node_t *front;
    queue_node_t *rear;
} queue_t;
int load_patterns_from_file(trie_node_t *root, const char *filepath);
void build_failure_links(trie_node_t *root);;
void trie_insert(trie_node_t *root, const char *pattern, const char *malware_name);
void trie_free(trie_node_t *root);
trie_node_t *trie_create(void);
#endif // TRIE_H