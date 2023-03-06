#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

BearerToken* newBearerToken(char* token, char* signature, enum purpose purpose,
                            time_t creation, time_t lifespan, size_t* size) {
    *size = strlen(token) + strlen(signature) + 2
          + sizeof(enum purpose) + sizeof(time_t) + sizeof(time_t);
    return &((BearerToken) {token, signature, purpose, creation, lifespan});
}

void bearerTokenToByteArray(BearerToken* token, BearerBytes* bytes) {
    size_t sizes[BEARER_PARAMS]       = { strlen(token->token) + 1, strlen(token->signature) + 1,
                                          sizeof(enum purpose), sizeof(time_t), sizeof(time_t) };
    const void* params[BEARER_PARAMS] = { token->token, token->signature, &token->purpose,
                                          &token->creation, &token->lifespan };
    size_t dest_offset  = 0;
    for (int i = 0; i < BEARER_PARAMS; i++) {
        memcpy(bytes + dest_offset, params[i], sizes[i]);
        dest_offset += sizes[i];
    }
}

BearerToken* byteArrayToBearerToken(BearerBytes* bytes) {
    size_t offset = 0;
    BearerToken* token = (BearerToken*) malloc(sizeof(BearerToken));

    token->token = (char*)(bytes + offset);
    offset += strlen(token->token) + 1;
    token->signature = (char*)(bytes + offset);
    offset += strlen(token->signature) + 1;
   
    memcpy(&token->purpose, bytes + offset,  sizeof(enum purpose));
    offset += sizeof(enum purpose);
    memcpy(&token->creation, bytes + offset, sizeof(time_t));
    offset += sizeof(time_t);
    memcpy(&token->lifespan, bytes + offset, sizeof(time_t));

#ifdef DEBUG
    /* %s%s%d%ld%ld format specifiers */
    printf("token: %s\nsignature: %s\npurpose: %d\ncreation: %ld\nlifespan: %ld\n",
            token->token, token->signature, token->purpose, token->creation, token->lifespan);
#endif
    return token; 
}

void dumpByteArray(BearerBytes* byteArray) {
    FILE* fp;
    fp = fopen("dump", "wb");
    for (size_t i = 0; i < sizeof(BearerToken); i++) {
        // todo: hash bytearray before dumping
        fprintf(fp, "%02X", byteArray[i]);
        printf("%02X", byteArray[i]);
    }

    fclose(fp);
}

#ifdef RMAIN
int main() {
    size_t         size;
    BearerToken*   token = newBearerToken("123456", "123456", GET, time(NULL), 3600, &size);
    BearerBytes* byteArray = (unsigned char*) malloc(size);

    bearerTokenToByteArray(token, byteArray);
    free(byteArray);

//  byteArrayToBearerToken(byteArray);
//  dumpByteArray(byteArray);

    return 0;
}
#endif
