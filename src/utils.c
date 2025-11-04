#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"

bool is_valid_path(const char *path) {
    if (path == NULL) return false;
    return access(path, F_OK) == 0;
}