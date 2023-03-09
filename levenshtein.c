#include <sqlite3ext.h>
SQLITE_EXTENSION_INIT1
#include <stdlib.h>

#ifdef __GNUC__
#define UNUSED(x) UNUSED ## x __attribute__((unused))
#else
#define UNUSED(x) UNUSED_ ## x
#endif

#define LEVENSHTEIN_MAX_STRLEN 1024

#define MIN(a,b) (((a) < (b)) ? (a) : (b))

static void levenshtein(sqlite3_context *context, int argc, sqlite3_value **argv);
static inline int levenshtein_distance(char const * restrict source, int sourceLen, char const * restrict target, int targetLen);

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
    int len1 = sqlite3_value_bytes(argv[0]);
    char const * const restrict str2 = (char const *) sqlite3_value_text(argv[1]);
    int len2 = sqlite3_value_bytes(argv[1]);
    int result = levenshtein_distance(str1, len1, str2, len2);

    if (result == -1) {
        sqlite3_result_null(context);
        return;
    }

    sqlite3_result_int(context, result);
}

// The distance computation itself
static inline int levenshtein_distance(char const * restrict source, int sourceLen, char const * restrict target, int targetLen)
{
    if (sourceLen > LEVENSHTEIN_MAX_STRLEN || targetLen > LEVENSHTEIN_MAX_STRLEN) {
        return -1;
    }

    if (sourceLen == 0 || targetLen == 0) {
        return sourceLen > targetLen ? sourceLen : targetLen;
    }

    int *d = (int *) malloc(sizeof(int) * (targetLen + 1));
    for (int i = 0; i < targetLen + 1; i++) {
        d[i] = i;
    }

    for (int i = 0; i < sourceLen; i++){
        char sourceChar = source[i];

        int substitutionPreCost = d[0]; // the diagonal in the previous row (doesn't include the cost of the substitution itself)
        int insertionCost = d[0]++; // the previous column

        for (int j = 1; j < targetLen + 1; j++){
            int deletionCost = d[j];

            int cost = substitutionPreCost;
            if (sourceChar != target[j - 1]) {
                cost = MIN(insertionCost, cost);
                cost = MIN(deletionCost, cost) + 1;
            }

            substitutionPreCost = deletionCost;
            insertionCost = cost;
            d[j] = insertionCost;
        }
    }

    int result = d[targetLen];
    free(d);
    return result;
}

