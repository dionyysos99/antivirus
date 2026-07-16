#include "../include/scanner.h"
#include "../include/hasher.h"
#include "../include/stats.h"
#include "../include/cli.h"
#include "../include/trie.h"
int main(int argc, char *argv[])
{
    cli_options_t opts = {0};
    if (parse_arguments(argc, argv, &opts) != 0)
    {
        return 1;
    }
    signature_db_t db;
    db_init(&db);
    if(db_load_from_file(&db, opts.sigdb_path) != 0)
    {
        fprintf(stderr, "İmza veritabanı yüklenemedi: %s\n", opts.sigdb_path);
        db_free(&db);
        return 1;
    }
    trie_node_t *root = trie_create();
    if(!root)
    {
        fprintf(stderr, "Trie olusturulamadi\n");
        db_free(&db);
        return 1;
    }
    if(opts.pattern_db_path != NULL)
    {
        load_patterns_from_file(root, opts.pattern_db_path);
        build_failure_links(root);
    }
    scan_stats_t stats = {0, 0, 0, 0, 0};
    scan_directory(opts.target_dir, &db, root, &stats, opts.verbose, opts.quiet);
    if(!opts.quiet)
    {
        printf("\n--- Tarama Ozeti ---\n");
        printf("Taranan dosya: %d\n", stats.files_scanned);
        printf("Atlanan dosya: %d\n", stats.files_skipped);
        printf("Atlanan dizin: %d\n", stats.dirs_skipped);
        printf("Tespit edilen tehdit: %d\n", stats.threats_found);
        printf("Atlanan sembolik link: %d\n", stats.symlinks_skipped);
    }
    db_free(&db);
    trie_free(root);
    return 0;
}