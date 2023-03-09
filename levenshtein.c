#include <sqlite3ext.h>
#include <string.h>
#include <stdlib.h>

#define LEVENSHTEIN_MAX_STRLEN 1024
// O(n * m) !!!

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

SQLITE_EXTENSION_INIT1

int levenshtein_distance(char*, char*);

static void levenFunc(sqlite3_context *context, int argc, sqlite3_value **argv)
{
    if (sqlite3_value_type(argv[0]) == SQLITE_NULL || sqlite3_value_type(argv[1]) == SQLITE_NULL){
        sqlite3_result_null(context);
        return;
    }

    int result = levenshtein_distance((char*) sqlite3_value_text(argv[0]), (char*) sqlite3_value_text(argv[1]));

    if (result == -1) {
        // one argument too long
        sqlite3_result_null(context);
        return;
    }

    sqlite3_result_int(context, result);
}

int sqlite3_extension_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi)
{
    SQLITE_EXTENSION_INIT2(pApi)
    sqlite3_create_function(db, "levenshtein", 2, SQLITE_ANY, 0, levenFunc, 0, 0);
    return 0;
}

int levenshtein_distance(char *s1, char *s2)
{
    int m = strlen(s1);
    int n = strlen(s2);

    if (m > LEVENSHTEIN_MAX_STRLEN || n > LEVENSHTEIN_MAX_STRLEN) {
        return -1;
    }

    if (m != 0 && n != 0) {
        n += 1;
        m += 1;
        int *d = (int *) malloc((sizeof(int)) * n * m);
        for (int k = 0; k < m; k++) {
            d[k] = k;
        }
        for (int k = 0; k < n; k++){
            d[k * m] = k;
        }
        for (int i = 1; i < m; i++){
            for (int j = 1; j < n; j++){
                int cost = s1[i - 1] == s2[j - 1] ? 0 : 1;
                d[j * m + i] = MIN(MIN(d[(j - 1) * m + i] + 1, d[j * m + i - 1] + 1), d[(j - 1) * m + i - 1] + cost);
            }
        }
        int result = d[m * n - 1];
        free(d);
        return result;
    } else {
        return MAX(m, n);
    }
}

