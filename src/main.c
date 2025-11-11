#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "args.h"
#include "utils.h"
# include <unistd.h>

typedef struct {
    char *path;
    int max_depth;
    long long min_size;
    int top;
    bool reverse;
    bool show_percent;
    bool show_summary;
} Options;

// valid path
bool is_valid_path(const char *path) {
    if (path == NULL) return false;
    return access(path, F_OK) == 0;
}

// min size str to number
long long str_to_num(const char *str) {
    long long result = 0;
    int i = 0;
    while (str[i] != '\0') {
      if (str[i] >= '0' && str[i] =< '9') {
      }
    }
}

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
            options->min_size = (argv[i]) + 11;
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

int main(int argc, char *argv[]) {

    printf("안녕하세요. dut-DiskUsageTree입니다.\n");
    printf("Arguments count: %d개\n", argc);

    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    Options options;
    parse_args(argc, argv, &options);

    printf("Path: %s\n", options.path);
    printf("Valid: %s\n", is_valid_path(options.path) ? "Yes" : "No");
    printf("Max depth: %d\n", options.max_depth);
    printf("Min size: %lld\n", options.min_size);
    printf("Top: %d\n", options.top);
    printf("Reverse: %s\n", options.reverse ? "true":"false");
    printf("Percent: %s\n", options.show_percent ? "true" : "false");
    printf("Summary: %s\n", options.show_summary ? "true" : "false");

    return 0;
}
