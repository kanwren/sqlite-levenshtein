CC = clang
LINKER_FLAGS = -dynamiclib -Wl,-flat_namespace -Wl,-undefined -Wl,suppress -Wl,-single_module
CFLAGS = -Wall -Wextra -O3

all: levenshtein.dylib

levenshtein.dylib: levenshtein.o
	$(CC) $(CFLAGS) $(LINKER_FLAGS) -o $@ $<

levenshtein.o: levenshtein.c
	$(CC) $(CFLAGS) -fno-common -DPIC -o $@ -c $<

.PHONY: clean
clean:
	rm -f levenshtein.o levenshtein.dylib

.PHONY: test check
check: levenshtein.dylib
	echo ".load levenshtein" | sqlite3
test: levenshtein.dylib
	time sqlite3 < ./levenshtein-test.sql
