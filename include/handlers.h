#ifndef __WEBC_HANDLERS_H_
#define __WEBC_HANDLERS_H_

#include <stddef.h>

#include "hash_table.h"

#include "request.h"
#include "response.h"

typedef void (*handler)(const WebcRequest *req, WebcResponse *res);

typedef struct HandlerNode HandlerNode;
typedef HashTable(HandlerNode *) HandlerChildren;

struct HandlerNode {
    handler func[WEBC_METHOD_UNKNOWN];
    HandlerChildren children;
};

typedef struct {
    HandlerNode *root;    
} Handlers;

void webc_handlers_init(Handlers *handlers);
    
void webc_handlers_insert(Handlers *handlers, const char *path, WebcMethod method, handler func);
handler webc_handlers_get(Handlers *handlers, const char *path, WebcMethod method);


#endif // __WEBC_HANDLERS_H_
