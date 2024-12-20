#define _GNU_SOURCE
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/utils.h"

#define MAX_PATTERNS 100
#define MAX_PATTERN_LENGTH 256
#define MAX_LINE_LENGTH 1024

void grep_file(const char *filename, CommonFlags *flags, bool multiple_files);

int main(int argc, char *argv[]) {
  CommonFlags flags = {0};
  int file_start = 0;
  bool used_args[argc];
  memset(used_args, 0, sizeof(used_args));
  parse_flags(argc, argv, &flags, &file_start, CMD_GREP, used_args);
  char *filenames[argc];
  size_t file_count = 0;
  if (file_start != -1) {
    for (int i = file_start; i < argc; ++i) {
      if (!(argv[i][0] == '-' && argv[i][1] != '\0')) {
        if (!used_args[i]) {
          filenames[file_count++] = argv[i];
        }
      }
    }
  }
  if (file_count == 0) {
    filenames[file_count++] = "-";
  }
  bool multiple_files = file_count > 1;
  for (size_t j = 0; j < file_count; ++j) {
    grep_file(filenames[j], &flags, multiple_files);
  }
  for (int i = 0; i < flags.grep_pattern_count; i++) {
    free(flags.grep_patterns[i]);
  }
  return 0;
}

void grep_file(const char *filename, CommonFlags *flags, bool multiple_files) {
  FILE *file = open_file(filename);
  if (!file) {
    if (!flags->grep_s) {
      fprintf(stderr, "s21_grep: %s: No such file or directory\n", filename);
    }
    return;
  }

  regex_t regexes[flags->grep_pattern_count];
  for (int i = 0; i < flags->grep_pattern_count; i++) {
    int reg_flags = REG_EXTENDED | (flags->grep_i ? REG_ICASE : 0);
    if (regcomp(&regexes[i], flags->grep_patterns[i], reg_flags) != 0) {
      fprintf(stderr, "Invalid regex: %s\n", flags->grep_patterns[i]);
      for (int j = 0; j <= i; j++) {
        regfree(&regexes[j]);
      }
      fclose(file);
      return;
    }
  }

  char line[MAX_LINE_LENGTH];
  int line_number = 0;
  int match_count = 0;
  bool file_has_match = false;
  while (fgets(line, sizeof(line), file)) {
    line_number++;
    bool match_found = false;
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
      line[len - 1] = '\0';
      len--;
    }
    for (int i = 0; i < flags->grep_pattern_count; i++) {
      if (regexec(&regexes[i], line, 0, NULL, 0) == 0) {
        match_found = true;
        break;
      }
    }

    if (flags->grep_v) {
      match_found = !match_found;
    }
    if (match_found) {
      match_count++;
      file_has_match = true;

      if (flags->grep_l) {
        break;
      }
      if (!flags->grep_c && !flags->grep_l) {
        if (flags->grep_o) {
          for (int i = 0; i < flags->grep_pattern_count; i++) {
            regex_t *regex = &regexes[i];
            regmatch_t pmatch;
            char *ptr = line;
            while (regexec(regex, ptr, 1, &pmatch, 0) == 0) {
              int start = pmatch.rm_so + (ptr - line);
              int end = pmatch.rm_eo + (ptr - line);
              if (pmatch.rm_eo > 0) {
                ptr += pmatch.rm_eo;
              } else {
                ptr++;
              }
              if (!flags->grep_h && multiple_files) {
                printf("%s:", filename);
              }
              if (flags->grep_n) {
                printf("%d:", line_number);
              }
              printf("%.*s\n", end - start, &line[start]);
            }
          }
        } else {
          if (!flags->grep_h && multiple_files) {
            printf("%s:", filename);
          }
          if (flags->grep_n) {
            printf("%d:", line_number);
          }
          printf("%s\n", line);
        }
      }
    }
  }
  if (flags->grep_c) {
    if (!flags->grep_h && multiple_files) {
      printf("%s:", filename);
    }
    printf("%d\n", match_count);
  }
  if (flags->grep_l && file_has_match) {
    printf("%s\n", filename);
  }
  for (int i = 0; i < flags->grep_pattern_count; i++) {
    regfree(&regexes[i]);
  }
  fclose(file);
}
