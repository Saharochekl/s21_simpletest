CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
INCLUDE = -I../common

TARGET = s21_grep

SRC = s21_grep.c ../common/utils.c

all: clean $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
test:
	sh test_func_grep.sh
	sh test_leak_grep.sh
rebuild: clean all