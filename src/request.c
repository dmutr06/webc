#include "request.h"
#include "lib.h"
#include <stdio.h>

WebcParseRequestResult webc_request_from_str(char *raw, WebcRequest *request) {
    char *raw_body = strstr(raw, "\r\n\r\n");
    if (raw_body) {
        request->body = raw_body + 4;
        *raw_body = '\0';
    } else {
        request->body = NULL;
    }

    char *raw_headers = strstr(raw, "\r\n");
    if (!raw_headers)
        return PARSE_REQ_ERROR;
    request->headers = webc_headers_from_str(raw_headers + 2);

    {
        char *raw_method = strchr(raw, ' ');
        if (!raw_method)
            return PARSE_REQ_ERROR;
        *raw_method = '\0';
        request->method = webc_method_from_str(raw);
        
        raw = raw_method + 1;

        char *path = strchr(raw, ' ');
        if (!path)
            return PARSE_REQ_ERROR;
        *path = '\0';
        request->path = raw;

        raw = path + 1;

        char *raw_ver = strstr(raw, "\r\n");
        if (!raw_ver)
            return PARSE_REQ_ERROR;
        *raw_ver = '\0';
        request->ver = webc_http_ver_from_str(raw);
    }

    return PARSE_REQ_OK;
}

void webc_request_deinit(WebcRequest *request) {
    ht_deinit(&request->headers);
}
