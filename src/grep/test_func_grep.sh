#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0
DIFF_RES=""

LOG_FILE="test_log.log"
> $LOG_FILE  # Очищаем лог-файл перед началом тестов

declare -a tests=(
"s test_0_grep.txt VAR"
"for s21_grep.c s21_grep.h Makefile VAR"
"for s21_grep.c VAR"
"-e for -e ^int s21_grep.c s21_grep.h Makefile VAR"
"-e for -e ^int s21_grep.c VAR"
"-e regex -e ^print s21_grep.c VAR -f test_ptrn_grep.txt"
"-e while -e void s21_grep.c Makefile VAR -f test_ptrn_grep.txt"
)

declare -a extra=(
"-n for test_1_grep.txt test_2_grep.txt"
"-n for test_1_grep.txt"
"-n -e ^\} test_1_grep.txt"
"-c -e /\ test_1_grep.txt"
"-ce ^int test_1_grep.txt test_2_grep.txt"
"-e ^int test_1_grep.txt"
"-nivh = test_1_grep.txt test_2_grep.txt"
"-e"
"-ie INT test_5_grep.txt"
"-echar test_1_grep.txt test_2_grep.txt"
"-ne = -e out test_5_grep.txt"
"-iv int test_5_grep.txt"
"-in int test_5_grep.txt"
"-c -l aboba test_1_grep.txt test_5_grep.txt"
"-v test_1_grep.txt -e ank"
"-noe ) test_5_grep.txt"
"-l for test_1_grep.txt test_2_grep.txt"
"-o -e int test_4_grep.txt"
"-e = -e out test_5_grep.txt"
"-noe ing -e as -e the -e not -e is test_6_grep.txt"
"-e ing -e as -e the -e not -e is test_6_grep.txt"
"-c -e . test_1_grep.txt -e '.'"
"-l for no_file.txt test_2_grep.txt"
"-f test_3_grep.txt test_5_grep.txt"
)

testing() {
    local t=$(echo $@ | sed "s/VAR/$var/")
    # Используем массивы для корректной передачи аргументов с пробелами
    args=($t)
    ./s21_grep "${args[@]}" > test_s21_grep.log 2>> $LOG_FILE
    grep "${args[@]}" > test_sys_grep.log 2>> $LOG_FILE
    DIFF_RES="$(diff -s test_s21_grep.log test_sys_grep.log)"
    (( COUNTER++ ))
    if [ "$DIFF_RES" == "Files test_s21_grep.log and test_sys_grep.log are identical" ]
    then
        (( SUCCESS++ ))
        echo -e "\033[32m$FAIL/$SUCCESS/$COUNTER success\033[0m grep $t"
    else
        (( FAIL++ ))
        echo -e "\033[31m$FAIL/$SUCCESS/$COUNTER fail\033[0m grep $t"
        echo "Test $COUNTER: grep $t" >> $LOG_FILE
        echo "$DIFF_RES" >> $LOG_FILE
        echo "--------------------------------------------------" >> $LOG_FILE
    fi
    rm -f test_s21_grep.log test_sys_grep.log
}

# специфические тесты
for i in "${extra[@]}"
do
    var="-"
    testing $i
done

# 1 параметр
for var1 in v c l n h o
do
    for i in "${tests[@]}"
    do
        var="-$var1"
        testing $i
    done
done

# 2 параметра
for var1 in v c l n h o
do
    for var2 in v c l n h o
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1 -$var2"
                testing $i
            done
        fi
    done
done

# 3 параметра
for var1 in v c l n h o
do
    for var2 in v c l n h o
    do
        for var3 in v c l n h o
        do
            if [ $var1 != $var2 ] && [ $var2 != $var3 ] && [ $var1 != $var3 ]
            then
                for i in "${tests[@]}"
                do
                    var="-$var1 -$var2 -$var3"
                    testing $i
                done
            fi
        done
    done
done

# 2 сдвоенных параметра
for var1 in v c l n h o
do
    for var2 in v c l n h o
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1$var2"
                testing $i
            done
        fi
    done
done

# 3 строенных параметра
for var1 in v c l n h o
do
    for var2 in v c l n h o
    do
        for var3 in v c l n h o
        do
            if [ $var1 != $var2 ] && [ $var2 != $var3 ] && [ $var1 != $var3 ]
            then
                for i in "${tests[@]}"
                do
                    var="-$var1$var2$var3"
                    testing $i
                done
            fi
        done
    done
done

echo -e "\033[31mFAIL: $FAIL\033[0m"
echo -e "\033[32mSUCCESS: $SUCCESS\033[0m"
echo "ALL: $COUNTER"
