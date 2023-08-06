#include <locale.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint8_t* key;
    uint8_t  offset;
    uint8_t  length;
} INDEX;

typedef struct {
    INDEX*  indexes;
    uint8_t dataSize;
    uint8_t tableSize; 
    uint8_t top;
} INDEX_FILE;

typedef union {
    char*    _string;
    uint8_t  _uint8_t;
    uint16_t _uint16_t;
    double   _double;
} type;

typedef enum {
    STRING,
    UINT8_T,
    UINT16_T,
    DOUBLE,
} type_state;

typedef struct {
    type       type;
    type_state state;
} value;

void blob(INDEX_FILE* index, FILE* buffer) {
    size_t   totalSize   = sizeof(uint8_t) * index->dataSize;
    uint8_t* indexStruct = (uint8_t*)malloc(totalSize);

    for (int i = 0; i < index->top; i++) {
        memcpy(indexStruct + index->indexes[i].offset,
               index->indexes[i].key, index->indexes[i].length);
    }

    fwrite(indexStruct, totalSize, 1, buffer);
    free(indexStruct);
}

size_t calculateOffset(value val) {
    switch (val.state) {
        case STRING:
            return strlen(val.type._string);
        case UINT8_T:
            return sizeof(uint8_t);
        case UINT16_T:
            return sizeof(uint16_t);
        case DOUBLE:
            return sizeof(double);
    };

    return 0;
}

void newValue_string   (const char* data);
void newValue_uint8_t  (uint8_t     data);
void newValue_uint16_t (uint16_t    data);
void newValue_double   (double      data);

#define newValue(_1, ...) _Generic((_1),                        \
        char*: newValue_string,      \
        uint8_t: newValue_uint8_t,   \
        uint16_t: newValue_uint16_t, \
        double: newValue_double)     \

int main() {
    FILE* buffer = fopen("out", "wb");

    INDEX_FILE index = {
        .indexes   = (INDEX*)malloc(sizeof(INDEX) * 10),
        .tableSize = 10,
        .top       = 0,
    };

    for (int i = 0; i < index.tableSize; i++) {
        char*    data_string   = "Hello World!";
        uint8_t  data_uint8_t  = 255;
        uint16_t data_uint16_t = 65535;
        double   data_double   = 3.14159;
        
        value val;

        val.type._string   = data_string;
//      val.type._uint8_t  = data_uint8_t;
//      val.type._uint16_t = data_uint16_t;
//      val.type._double   = data_double;

        val.state = STRING;

        size_t offset = calculateOffset(val);

        index.indexes[i].key    = (uint8_t*)&(val.type._string[0]);
        index.indexes[i].length = offset;
        index.indexes[i].offset = offset * i;

        index.dataSize += offset;
        index.top++;
    }

    blob(&index, buffer);

    free(index.indexes);
    fclose(buffer);

    return 0;
}
