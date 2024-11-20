#ifndef __WEBC_LIB_H_
#define __WEBC_LIB_H_

#include "dyn_arr.h"

typedef DynArr(const char *) WebcHeaders;

typedef enum {
  WEBC_HTTP_1_0,
  WEBC_HTTP_1_1,
  WEBC_HTTP_1_2,
  WEBC_HTTP_VER_UNKNOWN
} WebcHttpVersion;

typedef enum {
  WEBC_GET,
  WEBC_POST,
  WEBC_PUT,
  WEBC_DELETE,
  WEBC_PATCH,
  WEBC_OPTION,
  WEBC_METHOD_UNKNOWN
} WebcMethod;

typedef enum {
  WEBC_STATUS_OK = 200,
  WEBC_STATUS_CREATED = 201,
  WEBC_STATUS_BAD_REQUEST = 401,
  WEBC_STATUS_NOT_FOUND = 404
} WebcStatusCode;

WebcHttpVersion webc_http_ver_from_str(const char *raw);
const char *webc_http_ver_to_str(WebcHttpVersion ver);

WebcMethod webc_method_from_str(const char *raw);

WebcHeaders webc_headers_from_str(char *raw);

const char *webc_body_from_str(char *raw);

#endif // __WEBC_LIB_H_
