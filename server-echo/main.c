#include <errno.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#define UNIX_SOCK_PATH "/tmp/mysocket"

int main(int argc, char **argv) {
  struct event_base *base;
  struct evconnlistener *listener;
  struct sockaddr_un sin;

  /* Create new event base */
  base = event_base_new();
  if (!base) {
    puts("Couldn't open event base");
    return 1;
  }

  /* Clear the sockaddr before using it, in case there are extra
   * platform-specific fields that can mess us up. */
  memset(&sin, 0, sizeof(sin));
  sin.sun_family = AF_LOCAL;
  strcpy(sin.sun_path, UNIX_SOCK_PATH);

  /* Create a new listener */
  listener = evconnlistener_new_bind(base, accept_conn_cb, NULL,
                                     LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
                                     -1, (struct sockaddr *)&sin, sizeof(sin));
  if (!listener) {
    perror("Couldn't create listener");
    return 1;
  }
  evconnlistener_set_error_cb(listener, accept_error_cb);

  /* Lets rock */
  event_base_dispatch(base);
  return 0;
}
