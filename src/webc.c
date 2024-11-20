#include "webc.h"
#include "request.h"
#include "response.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#define WEBC_MAX_CONNS 128

static struct Webc {
  int fd;
  int port;
} webc;

void webc_init(void) {
  if ((webc.fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
  
  int opt = 1;
  if (setsockopt(webc.fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    perror("setsockopt failed");
    exit(EXIT_FAILURE);
  }
}

void webc_start(const int port) {
  struct sockaddr_in address = {0};

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  socklen_t addrlen = sizeof(address);

  if (bind(webc.fd, (struct sockaddr *) &address, addrlen) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(webc.fd, WEBC_MAX_CONNS) < 0) {
    perror("listen failed");
    exit(EXIT_FAILURE);
  }

  printf("Server has been started on port %d\n", port);

  while (1) {
    int new_socket = accept(webc.fd, (struct sockaddr *) &address, &addrlen);

    if (new_socket < 0) {
      perror("accept failed");
      exit(EXIT_FAILURE);
    }
    
    char raw_req[1024] = {0};
    ssize_t valread = read(new_socket, raw_req, sizeof(raw_req) - 1);
    (void) valread;

    WebcRequest req;
    webc_request_from_str(raw_req, &req);

    WebcResponse res;
    webc_response_init(new_socket, &res);

    if (req.method == WEBC_GET)
      res.body = "Hello, GET";
    else
      res.body = "Hello, not GET";

    char *raw_res = webc_response_to_str(&res);
    send(res._sock, raw_res, strlen(raw_res), 0);
    free(raw_res);

    printf("Sent a message\n");

    close(new_socket);
  }

  close(webc.fd);
}
