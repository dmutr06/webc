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
  if (!raw_headers) return PARSE_REQ_ERROR;
  request->headers = webc_headers_from_str(raw_headers + 2);
  
  {
    char *raw_method = strtok(raw, " ");
    if (!raw_method) return PARSE_REQ_ERROR;
    request->method = webc_method_from_str(raw_method);

    const char *path = strtok(NULL, " ");
    if (!path) return PARSE_REQ_ERROR;
    request->path = path; 

    char *raw_ver = strtok(NULL, "\r\n");
    if (!raw_ver) return PARSE_REQ_ERROR;
    request->ver = webc_http_ver_from_str(raw_ver);
  }

  return PARSE_REQ_OK;
}

