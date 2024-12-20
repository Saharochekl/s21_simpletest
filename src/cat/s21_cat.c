#define _GNU_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/utils.h"

void print_char(int c, CommonFlags flags);

int main(int argc, char *argv[]) {
  bool used_args[1000];
  memset(used_args, 0, sizeof(used_args));
  CommonFlags flags = {0};
  int file_start = 0;
  parse_flags(argc, argv, &flags, &file_start, CMD_CAT, used_args);
  if (file_start == argc) {
    argv[argc++] = "-";
  }
  for (int i = file_start; i < argc; ++i) {
    int line_num = 1;
    FILE *file;
    if (strcmp(argv[i], "-") == 0) {
      file = stdin;
    } else {
      file = fopen(argv[i], "r");
      if (!file) {
        perror(argv[i]);
        continue;
      }
    }
    int c;
    int prev_c = '\n';
    bool at_line_start = true;
    bool prev_line_blank = false;
    while ((c = fgetc(file)) != EOF) {
      if (flags.cat_s && c == '\n' && prev_c == '\n') {
        if (prev_line_blank) {
          prev_c = c;
          continue;
        } else {
          prev_line_blank = true;
        }
      } else if (c != '\n') {
        prev_line_blank = false;
      }
      if (at_line_start) {
        if (flags.cat_b) {
          if (c != '\n') {
            printf("%6d\t", line_num++);
          } else {
            if (flags.cat_e) {
              printf("      \t");
            }
          }
        } else if (flags.cat_n) {
          printf("%6d\t", line_num++);
        }
        at_line_start = false;
      }
      print_char(c, flags);
      if (c == '\n') {
        at_line_start = true;
      } else {
        at_line_start = false;
      }
      prev_c = c;
    }
  }
  for (int i = 0; i < flags.grep_pattern_count; i++) {
    free(flags.grep_patterns[i]);
  }
  return 0;
}

void print_char(int c, CommonFlags flags) {
  if (c == '\n') {
    if (flags.cat_e) {
      putchar('$');
    }
    putchar(c);
  } else if (c == '\t') {
    if (flags.cat_t) {
      printf("^I");
    } else {
      putchar(c);
    }
  } else if ((flags.cat_v || flags.cat_e || flags.cat_t) &&
             ((c < 32 && c != '\n' && c != '\t') || c == 127)) {
    if (c == 127) {
      printf("^?");
    } else {
      printf("^%c", c + 64);
    }
  } else {
    putchar(c);
  }
}
