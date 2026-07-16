#ifndef CLI_H
#define CLI_H

typedef struct
{
    char *target_dir;
    char *sigdb_path;
    int verbose;
    int quiet;
    char *pattern_db_path;
} cli_options_t;

int parse_arguments(int argc, char **argv, cli_options_t *opts);
void print_usage(const char *prog_name);

#endif