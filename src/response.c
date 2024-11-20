#include "response.h"
#include "dyn_arr.h"
#include "lib.h"
#include <stdio.h>
#include <string.h>

void webc_response_init(const int sock, WebcResponse *response) {
  response->_sock = sock; 
  response->ver = WEBC_HTTP_1_1;
  response->code = WEBC_STATUS_OK;
  WebcHeaders headers = darr_init(const char *, NULL);
  response->body = NULL;
  response->headers = headers;
}

char *webc_response_to_str(const WebcResponse *response) {
  char *raw_res = malloc(1024);

  const char *res_line =
    "%s %d %s\r\n";
  // TODO: code to str
  sprintf(raw_res, res_line,
    webc_http_ver_to_str(response->ver),
    response->code, "OK"
  );

  darr_foreach(&response->headers, header) {
    strcat(raw_res, *header);
    strcat(raw_res, "\r\n");
  }

  strcat(raw_res, "\r\n");

  if (response->body)
    strcat(raw_res, response->body);

  return raw_res;
}
