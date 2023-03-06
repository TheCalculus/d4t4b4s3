#include <time.h>

#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#endif

#define TOKEN_SIZE     6 * sizeof(char)
#define SIGNATURE_SIZE 6 * sizeof(char)
#define BEARER_PARAMS  5
#define TABLE_SIZE     1000

typedef unsigned char BearerBytes;

enum purpose {
    GET,    // retrieves resource without modifying
    POST,   // submits an entry to specified resource
    PUT,    // replaces all representations of target resource
    PATCH,  // applies partial modifications to a resource
    DELETE, // deletes specified resource
};

typedef struct {
    char*        token;
    char*        signature;
    enum purpose purpose;
    time_t       creation;
    time_t       lifespan;
} BearerToken;

