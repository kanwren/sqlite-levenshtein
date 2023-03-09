SQLITE3_BIN = /opt/homebrew/opt/sqlite3/bin/sqlite3
LINKER_FLAGS = -Wl,-flat_namespace -Wl,-undefined -Wl,suppress -Wl,-single_module

all: levenshtein.dylib

levenshtein.dylib: levenshtein.o
	gcc $(LINKER_FLAGS) -dynamiclib -O2 -o $@ $<

levenshtein.o: levenshtein.c
	gcc -g -O2 -fno-common -DPIC -o $@ -c $<

.PHONY: clean
clean:
	rm levenshtein.o levenshtein.dylib

.PHONY: test
test: levenshtein.dylib
	$(SQLITE3_BIN) < ./levenshtein-test.sql
