#define _GNU_SOURCE
#include "utils.h"

#include <stdlib.h>
#include <string.h>

void replace_characters(const char *input, char *output, char from,
                        const char *to) {
  while (*input) {
    if (*input == from) {
      strcat(output, to);
    } else {
      strncat(output, input, 1);
    }
    input++;
  }
}
bool is_empty_line(const char *line) {
  return strlen(line) == 1 && line[0] == '\n';
}

void parse_flags(int argc, char *argv[], CommonFlags *flags, int *file_start,
                 CommandType cmd_type, bool used_args[]) {
  bool expect_pattern = false;
  bool expect_file_with_patterns = false;
  bool non_option_encountered = false;
  *file_start = argc;
  for (int i = 1; i < argc; i++) {
    if (expect_pattern) {
      used_args[i] = true;
      if (cmd_type == CMD_GREP) {
        add_pattern(flags, argv[i]);
      }
      expect_pattern = false;
    } else if (expect_file_with_patterns) {
      used_args[i] = true;
      if (cmd_type == CMD_GREP) {
        load_patterns_from_file(argv[i], flags);
      }
      expect_file_with_patterns = false;
    } else if (argv[i][0] == '-' && argv[i][1] != '\0') {
      used_args[i] = true;
      if (cmd_type == CMD_CAT && !non_option_encountered) {
        for (char *c = &argv[i][1]; *c != '\0'; c++) {
          switch (*c) {
            case 'b':
              flags->cat_b = true;
              break;
            case 'e':
              flags->cat_e = true;
              flags->cat_v = true;
              break;
            case 'n':
              flags->cat_n = true;
              break;
            case 's':
              flags->cat_s = true;
              break;
            case 't':
              flags->cat_t = true;
              flags->cat_v = true;
              break;
            case 'v':
              flags->cat_v = true;
              break;
            default:
              fprintf(stderr, "Неизвестная опция для cat: -%c\n", *c);
          }
        }
      } else if (cmd_type == CMD_GREP) {
        int j = 1;
        while (argv[i][j] != '\0') {
          char opt = argv[i][j];
          switch (opt) {
            case 'e':
              flags->grep_e = true;
              if (argv[i][j + 1] != '\0') {
                add_pattern(flags, &argv[i][j + 1]);
                j = strlen(argv[i]) - 1;
              } else {
                expect_pattern = true;
              }
              break;
            case 'f':
              flags->grep_f = true;
              if (argv[i][j + 1] != '\0') {
                flags->f = strdup(&argv[i][j + 1]);
                load_patterns_from_file(flags->f, flags);
                j = strlen(argv[i]) - 1;
              } else {
                expect_file_with_patterns = true;
              }
              break;
            case 'i':
              flags->grep_i = true;
              break;
            case 'v':
              flags->grep_v = true;
              break;
            case 'c':
              flags->grep_c = true;
              break;
            case 'l':
              flags->grep_l = true;
              break;
            case 'n':
              flags->grep_n = true;
              break;
            case 'h':
              flags->grep_h = true;
              break;
            case 's':
              flags->grep_s = true;
              break;
            case 'o':
              flags->grep_o = true;
              break;
            default:
              fprintf(stderr, "Неизвестная опция для grep: -%c\n", opt);
          }
          j++;
        }
      }
    } else {
      if (*file_start > i) {
        *file_start = i;
      }
      non_option_encountered = true;
    }
  }
  if (expect_pattern || expect_file_with_patterns) {
    fprintf(stderr, "Ошибка: Отсутствует аргумент для опции.\n");
    return;
  }

  if (cmd_type == CMD_GREP && flags->grep_pattern_count == 0) {
    if (*file_start != argc) {
      add_pattern(flags, argv[*file_start]);
      (*file_start)++;
    } else {
      fprintf(stderr, "Ошибка: Отсутствует шаблон.\n");
      return;
    }
  }
  if (*file_start == argc) {
    *file_start = -1;
  }
}

FILE *open_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror(filename);
    return NULL;
  }
  return file;
}

bool read_line(FILE *file, char *buffer, size_t size) {
  return fgets(buffer, size, file) != NULL;
}

void add_pattern(CommonFlags *flags, const char *pattern) {
  if (flags->grep_pattern_count < MAX_PATTERNS) {
    flags->grep_patterns[flags->grep_pattern_count] = strdup(pattern);
    flags->grep_pattern_count++;
  } else {
    fprintf(stderr, "Ошибка: Слишком много шаблонов.\n");
    return;
  }
}

void load_patterns_from_file(const char *filename, CommonFlags *flags) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    if (!flags->grep_s) {
      fprintf(stderr, "s21_grep: %s: No such file or directory\n", filename);
    }
    return;
  }

  char buffer[MAX_PATTERN_LENGTH];
  while (fgets(buffer, sizeof(buffer), file)) {
    buffer[strcspn(buffer, "\n")] = '\0';
    add_pattern(flags, buffer);
  }

  fclose(file);
}
