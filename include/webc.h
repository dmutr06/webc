#ifndef __WEBC_H_
#define __WEBC_H_

#include "request.h"
#include "response.h"


void webc_init(void);
void webc_start(const int port);

void webc_route(const char *path, const WebcRequest *req, WebcResponse *res);

#endif // __WEBC_H_
