#ifndef SCANNER_H
#define SCANNER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include "../include/hash_table.h"
#include "../include/stats.h"
#include "../include/trie.h"
#include "../include/aho_corasick_search.h"
void scan_directory(const char *dir_path, signature_db_t *db, trie_node_t *pattern_trie,
                     scan_stats_t *stats, int verbose, int quiet);
#endif // SCANNER_H