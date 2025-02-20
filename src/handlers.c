#include "handlers.h"
#include "hash_table.h"
#include <stdio.h>

void handler_node_deinit(HandlerNode **node) {
    ht_deinit(&(*node)->children);
    free(*node);
}

void webc_handlers_init(Handlers *handlers) {
    handlers->root = malloc(sizeof(HandlerNode));
    memset(handlers->root, 0, sizeof(HandlerNode));
    ht_init(&handlers->root->children, handler_node_deinit);
}


void webc_handler_internal_insert(HandlerNode *node, char *full_path, WebcMethod method, handler func) {
    if (!full_path) return;

    size_t len = strlen(full_path);

    if (len == 0 || (len == 1 && *full_path == '/')) {
        node->func[method] = func;
        return;
    }

    char *path = strchr(full_path, '/');

    if (path == full_path)
        return webc_handler_internal_insert(node, path + 1, method, func);

    if (path != NULL)
        *path = '\0';

    HandlerNode **next = ht_get(&node->children, full_path);
    HandlerNode *next_node;
    if (!next) {
        next_node = malloc(sizeof(HandlerNode));
        memset(next_node, 0, sizeof(HandlerNode));
        ht_init(&next_node->children, handler_node_deinit);
        ht_insert(&node->children, full_path, next_node);
    } else next_node = *next;

    if (path == NULL) {
        next_node->func[method] = func;
        return;
    }
    
    return webc_handler_internal_insert(next_node, path + 1, method, func);
}

void webc_handlers_insert(Handlers *handlers, const char *key, WebcMethod method, handler func) {
    char buf[256];
    strcpy(buf, key);
    webc_handler_internal_insert(handlers->root, buf, method, func);
}

handler webc_handlers_internal_get(HandlerNode *node, char *full_path, WebcMethod method) {
    if (!full_path) return NULL;

    size_t len = strlen(full_path);

    if (len == 0 || (len == 1 && *full_path == '/')) return node->func[method];

    char *path = strchr(full_path, '/');

    if (path == full_path)
        return webc_handlers_internal_get(node, path + 1, method);

    if (path) {
        *path = '\0';
    }

    HandlerNode **next = ht_get(&node->children, full_path);

    if (!next) return NULL;

    return webc_handlers_internal_get(*next, !path ? "" : path + 1, method);
}

handler webc_handlers_get(Handlers *handlers, const char *path, WebcMethod method) {
    char buf[256];
    strcpy(buf, path);
    return webc_handlers_internal_get(handlers->root, buf, method); 
}
