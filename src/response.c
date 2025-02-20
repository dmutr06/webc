#include "response.h"
#include "lib.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

void webc_response_init(int sock, WebcResponse *response) {
    response->_sock = sock;
    response->ver = WEBC_HTTP_1_1;
    response->code = WEBC_STATUS_OK;
    ht_init(&response->headers, NULL);
    response->body = malloc(1024);
    memset(response->body, '\0', 1024);
}

char *webc_response_to_str(const WebcResponse *response) {
    char *raw_res = malloc(1024);

    const char *res_line = "%s %d %s\r\n";
    // TODO: code to str
    sprintf(raw_res, res_line, webc_http_ver_to_str(response->ver),
            response->code, webc_code_to_str(response->code));
    
    size_t total = 0;
    for (size_t i = 0; i < response->headers.cap || total < response->headers.size; ++i) {
        Bucket(const char *) *cur = (void *)response->headers.buckets + i;
        if (cur->state == HT_BUCKET_STATE_OCCUPIED) {
            strcat(raw_res, cur->key);
            strcat(raw_res, ": ");
            strcat(raw_res, cur->val);
            strcat(raw_res, "\r\n");
            total += 1;
        }
    }

    strcat(raw_res, "\r\n");

    if (*response->body != '\0')
        strcat(raw_res, response->body);

    return raw_res;
}

void webc_response_send(WebcResponse *response) {
    char length_header[32];
    if (*response->body != '\0') {
        sprintf(length_header, "%zu", strlen(response->body));
        ht_insert(&response->headers, "Content-Length", length_header);
    }
    char *raw_res = webc_response_to_str(response);
    send(response->_sock, raw_res, strlen(raw_res), 0);
    free(raw_res);
}

void webc_response_deinit(WebcResponse *response) {
    free(response->body);
    ht_deinit(&response->headers);
}
