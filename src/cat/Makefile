CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
INCLUDE = -I../common

TARGET = s21_cat

SRC = s21_cat.c ../common/utils.c



all: clean $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

clang:
	clang-format -n *.c *.h
	clang-format -i *.c *.h

test:
	sh test_func_cat.sh
	sh test_leak_cat.sh                     																							nemytyi myzskoi penis
rebuild: clean all