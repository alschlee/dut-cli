#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

bool match_pattern(const char *text, const char *pattern);
double calculate_percentage(long long size, long long total);
bool is_valid_path(const char *path);

#endif