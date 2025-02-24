#include "lib.h"
#include <stdio.h>
#include <string.h>

const char *webc_http_ver_to_str(WebcHttpVersion ver) {
    switch (ver) {
    case WEBC_HTTP_1_0:
        return "HTTP/1.0";
    case WEBC_HTTP_1_1:
        return "HTTP/1.1";
    case WEBC_HTTP_1_2:
        return "HTTP/1.2";
    default:
        return NULL;
    }
}

WebcHttpVersion webc_http_ver_from_str(const char *raw) {
    const char *http = "HTTP/1.";
    const size_t http_len = strlen(http);
    const size_t raw_len = strlen(raw);

    if (raw_len != http_len + 1)
        return WEBC_HTTP_VER_UNKNOWN;

    char *ver = strstr(raw, http);
    if (!ver)
        return WEBC_HTTP_VER_UNKNOWN;

    ver += http_len;

    switch (*ver) {
    case '0':
        return WEBC_HTTP_1_0;
    case '1':
        return WEBC_HTTP_1_1;
    case '2':
        return WEBC_HTTP_1_2;
    default:
        return WEBC_HTTP_VER_UNKNOWN;
    }
}

WebcMethod webc_method_from_str(const char *raw) {
#define CMP(a, b) (strcmp((a), (b)) == 0)
    if (CMP(raw, "GET"))
        return WEBC_GET;
    if (CMP(raw, "POST"))
        return WEBC_POST;
    if (CMP(raw, "PUT"))
        return WEBC_PUT;
    if (CMP(raw, "DELETE"))
        return WEBC_DELETE;
    if (CMP(raw, "PATCH"))
        return WEBC_PATCH;
    if (CMP(raw, "OPTION"))
        return WEBC_OPTION;

    return WEBC_METHOD_UNKNOWN;
#undef CMP
}

WebcHeaders webc_headers_from_str(char *raw) {
    WebcHeaders headers;
    ht_init(&headers, NULL);

    char *cur = raw;

    while (cur) {
        char *next = strstr(cur, "\r\n");
        if (next) {
            *next = '\0';
            next += 2;
        }
        char *delim = strstr(cur, ": ");
        if (!delim) continue;
        *delim = '\0';

        ht_insert(&headers, cur, delim + 2);
        cur = next;
    }

    return headers;
}

const char *webc_body_from_str(char *raw) { return raw; }

const char *webc_code_to_str(WebcStatusCode code) {
    switch (code) {
        case WEBC_STATUS_OK: return "OK";
        case WEBC_STATUS_CREATED: return "Created";
        case WEBC_STATUS_BAD_REQUEST: return "Bad Request";
        case WEBC_STATUS_UNAUTHORIZED: return "Unauthorized";
        case WEBC_STATUS_NOT_FOUND: return "Not Found";
        default: return NULL;
    }
}
