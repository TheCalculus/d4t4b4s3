#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define  IP_ADDR "127.0.0.1"
#define  PORT    8888

/* 
 * TODO:
 [√]  decent(?) error handling for debugging
 [ ]  implement hash table or trie structure to lookup bearer tokens
 [ ]  frequently dump bearer tokens to disk in case of failure
 [ ]  recover bearer tokens from disk after failure
 [ ]  add respectably secure authorization
 */

typedef struct {
    union {
        int   i_exp;
        char* s_exp;
    } value;
    enum {
        INTEGER,
        STRING,
    } valuetype;
} err;

void error(err expression, char* err) {
    switch (expression.valuetype) {
        case INTEGER:
            if (expression.value.i_exp >= 0) return;
            perror(err);
        case STRING:
            if (expression.value.s_exp != NULL
            || strlen(expression.value.s_exp) != 0) return;
    }
    exit(1);
}

int webserver() {
    int         socket_fd, client_fd;
    struct      sockaddr_in server, client;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    error((err) { .value.i_exp = socket_fd, .valuetype = INTEGER }, "failed to create socket");

    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = inet_addr(IP_ADDR);
    server.sin_port        = htons(PORT);

    error((err) { .value.i_exp = bind(socket_fd, (struct sockaddr *)&server, sizeof(server)), 
                  .valuetype   = INTEGER }, "failed to bind socket");

    listen(socket_fd, 3);
    int c = sizeof(struct sockaddr_in);

    while (1) {
        error((err) { .value.i_exp = (client_fd = accept(socket_fd, (struct sockaddr *)&client, (socklen_t *)&c)),
                      .valuetype   = INTEGER }, "accept failed");

        char buffer[2048];
        int  bytesRecv;
        int  bytesSent;

        error((err) { .value.i_exp = (bytesRecv = recv(client_fd, buffer, sizeof(buffer), 0)),
                      .valuetype   = INTEGER }, "receive failed");
        printf("received %d bytes from client:\n%s", bytesRecv, buffer);

        char* authHeader      = strstr(buffer, "Authorization: ");
        if (authHeader == NULL) 
        {   error((err) { .value.s_exp = authHeader,
                          .valuetype   = STRING }, "authHeader was NULL (no authentication provided)");  }

        char* authValue       = authHeader + strlen("Authorization: ");
        char* authType        = strtok(authValue, " ");
        char* authCredentials = strtok(authValue, " ");

        close(client_fd);
    }

    close(socket_fd);

    return 0;
}

int main() {
    webserver();
    return 0;
}
