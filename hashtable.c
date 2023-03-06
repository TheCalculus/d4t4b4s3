#include "definitions.h"
#include "bearer.c"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    BearerBytes* key;
    int          value;
} Bytepair;

typedef struct {
    Bytepair*    data;
    int          count;
    int          size;
} Bytebucket;

typedef struct {
    Bytebucket*  buckets;
    int          size;
} HashTable;

unsigned long hash(BearerBytes* str) {
    unsigned long hash = 5381;
    int c;
   
    while ((c = *str++) != 0)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % TABLE_SIZE;
}

Bytebucket createBytebucket() {
    Bytebucket bucket;
   
    bucket.data  = malloc(sizeof(Bytepair) * TABLE_SIZE);
    bucket.count = 0;
    bucket.size  = TABLE_SIZE;

    return bucket;
}

void freeBytebucket(Bytebucket* bucket) {
    free(bucket->data);

    bucket->count = 0;
    bucket->size  = 0;
}

void addBytepair(Bytebucket* bucket, BearerBytes* key, int value) {
    if (bucket->count >= bucket->size) {
        int newSize = bucket->size * 2;
        Bytepair* newData = malloc(sizeof(Bytepair) * newSize);
        
        for (int i = 0; i < bucket->count; i++) { newData[i] = bucket->data[i]; }

        free(bucket->data); 
        bucket->data = newData;
        bucket->size = newSize;
    }

    bucket->data[bucket->count].key = key;
    bucket->data[bucket->count].value = value;
    bucket->count++;
}

HashTable createHashTable() {
    HashTable table;
    
    table.buckets = malloc(sizeof(Bytebucket) * TABLE_SIZE);
    table.size = TABLE_SIZE;
    for (int i = 0; i < table.size; i++) { table.buckets[i] = createBytebucket(); }

    return table;
}

void freeHashTable(HashTable* table) {
    for (int i = 0; i < table->size; i++) {
        freeBytebucket(&table->buckets[i]);
    }
    free(table->buckets);
    table->size = 0;
}

void hashTableAdd(HashTable* table, BearerBytes* key, int value) {
    int index = hash(key);
    Bytebucket* bucket = &table->buckets[index];
    addBytepair(bucket, key, value);
}

int hashTableGet(HashTable* table, BearerBytes* key) {
    int index = hash(key);
    Bytebucket* bucket = &table->buckets[index];
    
    for (int i = 0; i < bucket->count; i++) {
        if (bucket->data[i].key == key) {
            return bucket->data[i].value;
        }
    }
   
    return -1;
}

int main() {
    HashTable table = createHashTable();

    size_t         size;
    BearerToken*   token   = newBearerToken("123456", "123456", GET, time(NULL), 3600, &size);
 
    BearerBytes* byteArray = (BearerBytes*) malloc(size);
    printf("%lu\n", hash(byteArray));

    bearerTokenToByteArray(token, byteArray);
   
    hashTableAdd(&table, byteArray, 65);
    hashTableAdd(&table, (BearerBytes*)'B', 66);

    printf("%d\n", hashTableGet(&table, byteArray));
    printf("%d\n", hashTableGet(&table, (BearerBytes*)'B'));
  
    freeHashTable(&table);
    return 0;
}
