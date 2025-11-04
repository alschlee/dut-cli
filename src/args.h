#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>

typedef struct {
    char *path;
    int max_depth;
    long long min_size;
    int top;
    bool reverse;
    bool show_percent;
    bool show_summary;
} Options;

void parse_args(int argc, char *argv[], Options *options);

#endif