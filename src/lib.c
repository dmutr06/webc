#include "lib.h"
#include <stdio.h>
#include <string.h>

const char *webc_http_ver_to_str(WebcHttpVersion ver) {
  switch (ver) {
    case WEBC_HTTP_1_0: return "HTTP/1.0";
    case WEBC_HTTP_1_1: return "HTTP/1.1";
    case WEBC_HTTP_1_2: return "HTTP/1.2";
    default: return NULL;
  }
}

WebcHttpVersion webc_http_ver_from_str(const char *raw) {
  const char *http = "HTTP/1.";
  const size_t http_len = strlen(http);
  const size_t raw_len = strlen(raw);

  if (raw_len != http_len + 1) return WEBC_HTTP_VER_UNKNOWN;

  char *ver = strstr(raw, http);
  if (!ver) return WEBC_HTTP_VER_UNKNOWN;

  ver += http_len;

  switch (*ver) {
    case '0': return WEBC_HTTP_1_0;
    case '1': return WEBC_HTTP_1_1;
    case '2': return WEBC_HTTP_1_2;
    default: return WEBC_HTTP_VER_UNKNOWN;
  }
}

WebcMethod webc_method_from_str(const char *raw) {
#define CMP(a, b) (strcmp((a), (b)) == 0)
  if (CMP(raw, "GET")) return WEBC_GET;
  if (CMP(raw, "POST")) return WEBC_POST;
  if (CMP(raw, "PUT")) return WEBC_PUT;
  if (CMP(raw, "DELETE")) return WEBC_DELETE;
  if (CMP(raw, "PATCH")) return WEBC_PATCH;
  if (CMP(raw, "OPTION")) return WEBC_OPTION;
  
  return WEBC_METHOD_UNKNOWN;
#undef CMP
}

WebcHeaders webc_headers_from_str(char *raw) {
  WebcHeaders headers = darr_init(const char *, NULL);
  const char *token = strtok(raw, "\r\n");

  while (token != NULL) {
    darr_push(&headers, token);
    token = strtok(NULL, "\r\n");
  }

  return headers;
}

const char *webc_body_from_str(char *raw) {
  return raw;
}

