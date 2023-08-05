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

int main() {
    FILE* buffer = fopen("out", "wb");

    INDEX_FILE index = {
        .indexes   = (INDEX*)malloc(sizeof(INDEX) * 10),
        .tableSize = 10,
        .top       = 0,
    };

    for (int i = 0; i < index.tableSize; i++) {
        char* data = "Hello World!";

        index.indexes[i].key    = (uint8_t*)&(data[0]);
        index.indexes[i].length = (uint8_t)(strlen(data));
        index.indexes[i].offset = (uint8_t)(strlen(data) * i);

        index.dataSize += strlen(data);
        index.top++;
    }

    blob(&index, buffer);

    free(index.indexes);
    fclose(buffer);

    return 0;
}

