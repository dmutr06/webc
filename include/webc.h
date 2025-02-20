#ifndef __WEBC_H_
#define __WEBC_H_

#include "request.h"
#include "response.h"
#include "handlers.h"

void webc_init(void);
void webc_start(const int port);

void webc_route(WebcMethod method, const char *path, handler func);

#endif // __WEBC_H_
