#include <sqlite3ext.h>
SQLITE_EXTENSION_INIT1
#include <string.h>
#include <stdlib.h>

#ifdef __GNUC__
#define UNUSED(x) UNUSED ## x __attribute__((unused))
#else
#define UNUSED(x) UNUSED_ ## x
#endif

#define LEVENSHTEIN_MAX_STRLEN 1024

#define MIN(a,b) (((a) < (b)) ? (a) : (b))

static void levenshtein(sqlite3_context *context, int argc, sqlite3_value **argv);
static inline int levenshtein_distance(char const * const restrict s1, char const * const restrict s2);

// Initialize the extension
int sqlite3_levenshtein_init(sqlite3 *db, char **UNUSED(pzErrMsg), const sqlite3_api_routines *pApi)
{
    SQLITE_EXTENSION_INIT2(pApi)
    sqlite3_create_function(db, "levenshtein", 2, SQLITE_ANY, 0, levenshtein, 0, 0);
    return SQLITE_OK;
}

// The levenshtein sqlite function
static void levenshtein(sqlite3_context *context, int UNUSED(argc), sqlite3_value **argv)
{
    if (sqlite3_value_type(argv[0]) == SQLITE_NULL || sqlite3_value_type(argv[1]) == SQLITE_NULL){
        sqlite3_result_null(context);
        return;
    }

    char const * const restrict str1 = (char const *) sqlite3_value_text(argv[0]);
    char const * const restrict str2 = (char const *) sqlite3_value_text(argv[1]);
    int result = levenshtein_distance(str1, str2);

    if (result == -1) {
        sqlite3_result_null(context);
        return;
    }

    sqlite3_result_int(context, result);
}

// The distance computation itself
static inline int levenshtein_distance(char const * const restrict s1, char const * const restrict s2)
{
    int m = strlen(s1);
    int n = strlen(s2);

    if (m > LEVENSHTEIN_MAX_STRLEN || n > LEVENSHTEIN_MAX_STRLEN) {
        return -1;
    }

    if (m == 0 || n == 0) {
        return m > n ? m : n;
    }

    m += 1;
    n += 1;

    int *d = (int *) malloc(sizeof(int) * (n * m));

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
}

