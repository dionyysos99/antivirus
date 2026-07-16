#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cli.h"


void print_usage(const char *prog_name)
{
    fprintf(stderr, "Kullanim: %s -d <dizin> -s <imza_dosyasi> [secenekler]\n\n", prog_name);
    fprintf(stderr, "Zorunlu:\n");
    fprintf(stderr, "  -d, --dir <dizin>       Taranacak dizin\n");
    fprintf(stderr, "  -s, --sigdb <dosya>     Imza veritabani (CSV)\n\n");
    fprintf(stderr, "Secenekler:\n");
    fprintf(stderr, "  -v, --verbose           Her dosya icin detayli cikti goster\n");
    fprintf(stderr, "  -q, --quiet             Sadece tehdit ve ozet cikti goster\n");
    fprintf(stderr, "  -h, --help              Bu yardim mesajini goster\n");
    fprintf(stderr, "  -p, --patterns <dosya>  Pattern veritabani (opsiyonel, CSV)\n");
}

int parse_arguments(int argc, char **argv, cli_options_t *opts)
{
    opts->pattern_db_path = NULL;
    opts->target_dir = NULL;
    opts->sigdb_path = NULL;
    opts->verbose = 0;
    opts->quiet = 0;

    static struct option long_options[] = {
        {"dir",     required_argument, 0, 'd'},
        {"sigdb",   required_argument, 0, 's'},
        {"verbose", no_argument,       0, 'v'},
        {"quiet",   no_argument,       0, 'q'},
        {"help",    no_argument,       0, 'h'},
        {"patterns", required_argument, 0, 'p'},

        {0, 0, 0, 0}  
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "d:s:p:vqh", long_options, &option_index)) != -1)
    {
        switch (opt)
        {
            case 'd':
                opts->target_dir = optarg;
                break;
            case 's':
                opts->sigdb_path = optarg;
                break;
            case 'v':
                opts->verbose = 1;
                break;
            case 'q':
                opts->quiet = 1;
                break;
            case 'p':
            opts->pattern_db_path = optarg;
            break;
            case 'h':
                print_usage(argv[0]);
                exit(EXIT_SUCCESS);
            case '?':
                print_usage(argv[0]);
                return -1;
            default:
                return -1;
        }
    }
    if (opts->target_dir == NULL || opts->sigdb_path == NULL)
    {
        fprintf(stderr, "Hata: -d ve -s zorunlu.\n\n");
        print_usage(argv[0]);
        return -1;
    }

    if (opts->verbose && opts->quiet)
    {
        fprintf(stderr, "Hata: -v ve -q birlikte kullanilamaz.\n");
        return -1;
    }

    return 0;
}