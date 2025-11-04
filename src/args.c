#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "args.h"

void parse_args(int argc, char *argv[], Options *options) {
    options->path = ".";
    options->max_depth = -1;
    options->min_size = 0;
    options->top = 0;
    options->reverse = false;
    options->show_percent = false;
    options->show_summary = false;
    
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--max-depth=", 12) == 0) {
            options->max_depth = atoi(argv[i] + 12);
        } else if (strncmp(argv[i], "--min-size=", 11) == 0) {
            options->min_size = parse_size(argv[i] + 11);
        } else if (strncmp(argv[i], "--top=", 6) == 0) {
            options->top = atoi(argv[i] + 6);
        } else if (strcmp(argv[i], "--reverse") == 0) {
            options->reverse = true;
        } else if (strcmp(argv[i], "--percent") == 0) {
            options->show_percent = true;
        } else if (strcmp(argv[i], "--summary") == 0) {
            options->show_summary = true;
        } else if (argv[i][0] != '-') {
            options->path = argv[i];
        }
    }
}