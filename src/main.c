#include <stdio.h>
#include "hash_table.h"
#include "webc.h"

#define PORT 6969 

bool read_file(const char *path, char buf[1024]) {
    FILE *file = fopen(path, "r");
    if (!file) return false;

    memset(buf, '\0', 1024);
    fread(buf, 1, 1023, file);

    fclose(file);

    return true;
}

void get_home(const WebcRequest *req, WebcResponse *res) {
    (void) req;
    char buf[1024];
    read_file("index.html", buf);
    strcpy(res->body, buf);
    ht_insert(&res->headers, "Content-Type", "text/html");
    webc_response_send(res);
}

void get_about(const WebcRequest *req, WebcResponse *res) {
    strcpy(res->body, *ht_get(&req->headers, "Host"));
    webc_response_send(res);
}

void post_root(const WebcRequest *req, WebcResponse *res) {
    printf("received some data...\n");
    sprintf(res->body, "Your data: |%s| has beed read", req->body);


    webc_response_send(res);
}

int main(void) {
    webc_init();
    webc_route(WEBC_GET, "/", get_home);
    webc_route(WEBC_GET, "/about", get_about);
    webc_route(WEBC_POST, "/post", post_root);
    printf("Server has been started on port %d\n", PORT);
    webc_start(PORT);
}
