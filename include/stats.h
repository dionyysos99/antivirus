#ifndef STATS_H
#define STATS_H
typedef struct
{
    int files_scanned;
    int files_skipped;
    int dirs_skipped;
    int threats_found;
    int symlinks_skipped;
} scan_stats_t;
#endif // STATS_H