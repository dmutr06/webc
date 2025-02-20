#ifndef __WEBC_RESPONSE_H_
#define __WEBC_RESPONSE_H_

#include "lib.h"

typedef struct {
    int _sock;
    WebcHttpVersion ver;
    WebcStatusCode code;
    WebcHeaders headers;
    char *body;
} WebcResponse;

void webc_response_init(const int sock, WebcResponse *response);

char *webc_response_to_str(const WebcResponse *response);
void webc_response_send(WebcResponse *response);
void webc_response_deinit(WebcResponse *response);

#endif // __WEBC_RESPONSE_H_
