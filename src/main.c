#include <stdio.h>
#include "hash_table.h"
#include "webc.h"

#define PORT 6969 

void get_home(const WebcRequest *req, WebcResponse *res) {
    (void) req;
    strcpy(res->body, "Home");
    webc_response_send(res);
}

void get_about(const WebcRequest *req, WebcResponse *res) {
    strcpy(res->body, *ht_get(&req->headers, "Host"));
    webc_response_send(res);
}

void post_root(const WebcRequest *req, WebcResponse *res) {
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
