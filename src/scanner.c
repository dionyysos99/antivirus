#include "../include/scanner.h"
#include "../include/hasher.h"
#include "../include/stats.h"
#include "../include/trie.h"
#include "../include/aho_corasick_search.h"
#define MAX_PATH_LENGTH 4096

void scan_directory(const char *dir_path, signature_db_t *db, trie_node_t *pattern_trie, scan_stats_t *stats, int verbose, int quiet)
{
    DIR *dir = opendir(dir_path);
    if (!dir)
    {
        if (errno == EACCES)
            fprintf(stderr, "[atlanildi] Izin reddedildi: %s\n", dir_path);
        else if (errno == ENOENT)
            fprintf(stderr, "[atlanildi] Dizin bulunamadi: %s\n", dir_path);
        else
            fprintf(stderr, "[atlanildi] %s: %s\n", dir_path, strerror(errno));

        stats->dirs_skipped++;
        return;
    }

    struct dirent *entry;
    errno = 0;  // readdir hata ayrımı için sıfırlıyoruz (aşağıda açıklıyorum)

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            errno = 0;
            continue;
        }

        char full_path[MAX_PATH_LENGTH];
        int written = snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        if (written < 0 || (size_t)written >= sizeof(full_path))
        {
            fprintf(stderr, "[atlanildi] Yol cok uzun: %s/%s\n", dir_path, entry->d_name);
            stats->files_skipped++;
            errno = 0;
            continue;
        }

        struct stat st;
        if (lstat(full_path, &st) != 0)
        {
            if (errno == EACCES)
                fprintf(stderr, "[atlanildi] Izin reddedildi: %s\n", full_path);
            else if (errno == ENOENT)
                fprintf(stderr, "[atlanildi] Bulunamadi (bozuk symlink olabilir): %s\n", full_path);
            else
                fprintf(stderr, "[atlanildi] %s: %s\n", full_path, strerror(errno));

            stats->files_skipped++;
            errno = 0;
            continue;
        }
        if (S_ISLNK(st.st_mode))
        {
        printf("[atlandi] Symlink takip edilmiyor: %s\n", full_path);
        stats->symlinks_skipped++;
        errno = 0;
        continue;
        }
        if (S_ISDIR(st.st_mode))
        {
            scan_directory(full_path, db, pattern_trie, stats, verbose, quiet);
        }
        else if (S_ISREG(st.st_mode))
        {
            char hash_hex[SHA256_HEX_LEN];

            if (hash_file(full_path, hash_hex) != 0)
            {
                fprintf(stderr, "[atlanildi] Hashlenemedi: %s\n", full_path);
                stats->files_skipped++;
                continue;
            }

            stats->files_scanned++;

            signature_entry_t *hash_match = db_lookup(db, hash_hex);
            if (hash_match)
            {
                printf("[!] MALWARE TESPIT EDILDI (hash): %s -> %s\n",
                full_path, hash_match->malware_name);
                stats->threats_found++;
                continue;  
            }

    
            if (pattern_trie != NULL)
            {
            match_list_t results;
            if (aho_corasick_search(pattern_trie, full_path, &results) == 0 && results.match_count > 0)
            {
                printf("[!] MALWARE TESPIT EDILDI (pattern): %s -> %s (ilk eslesme, offset %ld)\n",
                full_path, results.matches[0].pattern_name, results.matches[0].offset);
                stats->threats_found++;
                continue;
            }
        }

        if (verbose && !quiet)
        printf("[ok] Temiz: %s\n", full_path);
        }

        errno = 0;
    }

    // readdir NULL döndü - bu normal bitiş mi, hata mı?
    if (errno != 0)
    {
        fprintf(stderr, "[uyari] %s dizini tam okunamadi: %s\n", dir_path, strerror(errno));
    }

    closedir(dir);
}