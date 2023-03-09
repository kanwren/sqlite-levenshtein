SQLITE3_BIN = /opt/homebrew/opt/sqlite3/bin/sqlite3
LINKER_FLAGS = -dynamiclib -Wl,-flat_namespace -Wl,-undefined -Wl,suppress -Wl,-single_module
CFLAGS = -Wall -Wextra -O3

all: levenshtein.dylib

levenshtein.dylib: levenshtein.o
	gcc $(CFLAGS) $(LINKER_FLAGS) -o $@ $<

levenshtein.o: levenshtein.c
	gcc $(CFLAGS) -fno-common -DPIC -o $@ -c $<

.PHONY: clean
clean:
	rm levenshtein.o levenshtein.dylib

.PHONY: test check
check: levenshtein.dylib
	echo ".load levenshtein" | $(SQLITE3_BIN)
test: levenshtein.dylib
	time $(SQLITE3_BIN) < ./levenshtein-test.sql
