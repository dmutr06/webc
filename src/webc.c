#include "webc.h"

#include "request.h"
#include "response.h"
#include "handlers.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define __WEBC_MAX_CONNS 128

#define __WEBC_REQ_BUF_SIZE 4096

static struct Webc {
    int fd;
    int port;
    Handlers handlers;
} webc;


void webc_init(void) {
    if ((webc.fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(webc.fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt failed");
        close(webc.fd);
        exit(EXIT_FAILURE);
    }

    webc_handlers_init(&webc.handlers);
}

void webc_start(const int port) {
    #define DEFER(code) exit_code = code; goto defer;

    int exit_code = EXIT_SUCCESS;
    struct sockaddr_in address = {0};

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    socklen_t addrlen = sizeof(address);

    if (bind(webc.fd, (struct sockaddr *)&address, addrlen) < 0) {
        perror("bind failed");
        DEFER(EXIT_FAILURE);
    }

    if (listen(webc.fd, __WEBC_MAX_CONNS) < 0) {
        perror("listen failed");
        DEFER(EXIT_FAILURE);
    }

    while (1) {
        int new_socket = accept(webc.fd, (struct sockaddr *)&address, &addrlen);

        if (new_socket < 0) {
            perror("accept failed");
            continue;
        }

        char *raw_req = malloc(__WEBC_REQ_BUF_SIZE);
        ssize_t valread = read(new_socket, raw_req, __WEBC_REQ_BUF_SIZE - 1);
        (void)valread;
        
        WebcRequest req;
        webc_request_from_str(raw_req, &req);

        WebcResponse res;
        webc_response_init(new_socket, &res);
        
        bool accepted = false;

        handler func = webc_handlers_get(&webc.handlers, req.path, req.method);

        if (func) {
            func(&req, &res);
            accepted = true;
        }

        if (!accepted) {
            res.code = WEBC_STATUS_NOT_FOUND;
            webc_response_send(&res);
        }

        webc_request_deinit(&req);
        webc_response_deinit(&res);

        free(raw_req);
    
        close(new_socket);
    }
    
    defer:
    close(webc.fd);
    exit(exit_code);
    #undef DEFER
}

void webc_route(WebcMethod method, const char *path, handler func) {
    webc_handlers_insert(&webc.handlers, path, method, func);
}
