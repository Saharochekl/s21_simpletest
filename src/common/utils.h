#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdio.h>

#define MAX_PATTERNS 100
#define MAX_PATTERN_LENGTH 256

typedef struct {
  bool cat_b, cat_e, cat_n, cat_s, cat_t, cat_v;
  bool grep_e, grep_i, grep_v, grep_c, grep_l, grep_n, grep_h, grep_s, grep_f,
      grep_o;
  char *f;
  char *grep_patterns[MAX_PATTERNS];
  int grep_pattern_count;
} CommonFlags;

typedef enum { CMD_CAT, CMD_GREP } CommandType;

void parse_flags(int argc, char *argv[], CommonFlags *flags, int *file_start,
                 CommandType cmd_type, bool used_args[]);
FILE *open_file(const char *filename);
bool read_line(FILE *file, char *buffer, size_t size);
bool is_empty_line(const char *line);
void replace_characters(const char *input, char *output, char from,
                        const char *to);

void add_pattern(CommonFlags *flags, const char *pattern);
void load_patterns_from_file(const char *filename, CommonFlags *flags);
#endif  // UTILS_H
