#ifndef __WEBC_REQUEST_H_
#define __WEBC_REQUEST_H_

#include "lib.h"

typedef struct {
  WebcMethod method;
  const char *path;
  WebcHttpVersion ver;
  WebcHeaders headers;
  const char *body;
} WebcRequest;

typedef enum {
  PARSE_REQ_OK,
  PARSE_REQ_ERROR
} WebcParseRequestResult;

WebcParseRequestResult webc_request_from_str(char *raw, WebcRequest *request);

#endif // __WEBC_REQUEST_H_
