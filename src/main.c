#include <stdio.h>
#include <stdlib.h>
#include "args.h"
#include "utils.h"

int main(int argc, char *argv[]) {

    printf("안녕하세요. dut-DiskUsageTree입니다.\n");
    printf("Arguments count: %d개\n", argc);

    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    Options options;
    parse_args(argc, argv, &options);

    printf("Path: %s\n", options.path);
    printf("Max depth: %d\n", options.max_depth);
    printf("Min size: %ld\n", options.min_size);
    printf("Top: %d\n", options.top);
    printf("Reverse: %s\n", options.reverser ? "true":"false");
    printf("Percent: %s\n", options.show_percent ? "true" : "false");
    printf("Summary: %s\n", options.shows_summary ? "true" : "false");

    return 0;
}