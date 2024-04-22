# Makefile
# Solution IOS-Project-2, 12.4.2024 
# Author: Michal Repčík, FIT
# Compiled: gcc 11.4.0

CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic -pthread

TARGETS = proj2

.PHONY: all clean test1 test2 zip cleanzip

all: $(TARGETS)

proj2: proj2.c
	$(CC) $(CFLAGS) $^ -o $@

# Basic test
test1:
# 			L Z K TL TB
	./proj2 8 4 10 4 5 && cat proj2.out
# Capacity test
test2:
# 			L  Z K TL TB
	./proj2 18 4 10 4 5 && cat proj2.out

clean:
	rm -f $(TARGETS) *.out

zip:
	zip proj2.zip *.c Makefile

cleanzip:
	rm -f proj2.zip