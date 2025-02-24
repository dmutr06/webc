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

void webc_start(int port) {
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
        struct sockaddr_in client_addr;
        socklen_t client_addrlen = 0;
        int client_fd = accept(webc.fd, (struct sockaddr *)&client_addr, &client_addrlen);

        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        char raw_req[__WEBC_REQ_BUF_SIZE + 1];
        memset(raw_req, '\0', sizeof(raw_req));
        ssize_t valread = read(client_fd, raw_req, __WEBC_REQ_BUF_SIZE);
        (void)valread;
        
        WebcRequest req;
        webc_request_from_str(raw_req, &req);

        WebcResponse res;
        webc_response_init(client_fd, &res);
        
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

        shutdown(client_fd, SHUT_RDWR); 
        close(client_fd);

        webc_request_deinit(&req);
        webc_response_deinit(&res);
    }
    
    defer:
    close(webc.fd);
    exit(exit_code);
    #undef DEFER
}

void webc_route(WebcMethod method, const char *path, handler func) {
    webc_handlers_insert(&webc.handlers, path, method, func);
}
