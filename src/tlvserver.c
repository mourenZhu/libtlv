#include "tlv/tlvserver.h"
#include <string.h>
#include <arpa/inet.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <stdio.h>
#include "tlv//log.h"
#include "tlv/tlv.h"
#include "tlv/tlvevent.h"

int tlvsconf_init(TLVServerConf *tlvServerConf, const char *inet_addr, const char *inet_addr6, u_int16_t port, u_int8_t inet_type)
{
    strcpy(tlvServerConf->inet_addr, inet_addr);
    strcpy(tlvServerConf->inet_addr6, inet_addr6);
    tlvServerConf->port = port;
    tlvServerConf->inet_type = inet_type;
    return 0;
}

static void
server_read_cb(struct bufferevent *bev, void *ctx)
{
    struct evbuffer *input = bufferevent_get_input(bev);
    log_debug("read cd buffer length: %ld", evbuffer_get_length(input));
    TLV *tlv = NULL;
    TLVServer *tlvServer = (TLVServer *)ctx;
    while ((tlv = tlv_read_new_with_bufferevent(bev))) {
        if (tlvServer->tlv_handler) {
            tlvServer->tlv_handler(tlv, bev, NULL);
        }
        tlv_free(&tlv);
    }
}

static void
server_event_cb(struct bufferevent *bev, short events, void *ctx)
{
    TLVServer *tlvServer = (TLVServer *)ctx;
    if (tlvServer->event_handler) {
        tlvServer->event_handler(bev, events, NULL);
    }
    if (events & BEV_EVENT_ERROR)
        log_error("Error from bufferevent");
    if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
        log_debug("events: %d ,event close", events);
        bufferevent_free(bev);
    }
}

static void
accept_conn_cb(struct evconnlistener *listener,
               evutil_socket_t fd, struct sockaddr *address, int socklen,
               void *ctx)
{
    struct sockaddr_in *sin = (struct sockaddr_in*) address;
    char client_ip_addr[INET_ADDRSTRLEN];
    const char *ip_ptr = inet_ntop(AF_INET, &(sin->sin_addr), client_ip_addr, INET_ADDRSTRLEN);
    if (ip_ptr) {
        log_info("new conn, fd: %d, ip: %s, port: %d", fd, client_ip_addr, sin->sin_port);
    } else {
        log_error("can't get client ip");
    }

    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev = bufferevent_socket_new(
            base, fd, BEV_OPT_CLOSE_ON_FREE);
    TLVServer *tlvServer = (TLVServer *)ctx;

//    struct timeval timeout_read = {.tv_sec = 5};
//    bufferevent_set_timeouts(bev, &timeout_read, NULL);

    bufferevent_setcb(bev, server_read_cb, NULL, server_event_cb, tlvServer);

    bufferevent_enable(bev, EV_READ|EV_WRITE);

    if (tlvServer->accept_conn_handler) {
        tlvServer->accept_conn_handler(bev, address, socklen, NULL);
    }
}

static void
accept_error_cb(struct evconnlistener *listener, void *ctx)
{
    struct event_base *base = evconnlistener_get_base(listener);
    int err = EVUTIL_SOCKET_ERROR();
    log_error("Got an error %d (%s) on the listener. "
              "Shutting down.\n", err, evutil_socket_error_to_string(err));
    TLVServer *tlvServer = (TLVServer *)ctx;
    if (tlvServer->accept_error_handler) {
        tlvServer->accept_error_handler(NULL);
    }
    event_base_loopexit(base, NULL);
}


static void
server_read_cb_v6(struct bufferevent *bev, void *ctx)
{
    struct evbuffer *input = bufferevent_get_input(bev);
    log_debug("v6 read cd buffer length: %ld", evbuffer_get_length(input));
    TLV *tlv = NULL;
    TLVServer *tlvServer = (TLVServer *)ctx;
    while ((tlv = tlv_read_new_with_bufferevent(bev))) {
        if (tlvServer->tlv_handler_v6) {
            tlvServer->tlv_handler_v6(tlv, bev, NULL);
        }
        tlv_free(&tlv);
    }
}

static void
server_event_cb_v6(struct bufferevent *bev, short events, void *ctx)
{
    TLVServer *tlvServer = (TLVServer *)ctx;
    if (tlvServer->event_handler_v6) {
        tlvServer->event_handler_v6(bev, events, NULL);
    }
    if (events & BEV_EVENT_ERROR)
        log_error("v6 Error from bufferevent");
    if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
        log_debug("v6 events: %d ,event close", events);
        bufferevent_free(bev);
    }
}


static void
accept_conn_cb_v6(struct evconnlistener *listener,
               evutil_socket_t fd, struct sockaddr *address, int socklen,
               void *ctx)
{
    struct sockaddr_in6 *sin6 = (struct sockaddr_in6*) address;
    char client_ip_addr[INET6_ADDRSTRLEN];
    const char *ip_ptr = inet_ntop(AF_INET6, &(sin6->sin6_addr), client_ip_addr, INET6_ADDRSTRLEN);
    if (ip_ptr) {
        log_info("ipv6 new conn, fd: %d, ip: %s, port: %d", fd, client_ip_addr, sin6->sin6_port);
    } else {
        log_error("can't get client ip");
    }

    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev = bufferevent_socket_new(
            base, fd, BEV_OPT_CLOSE_ON_FREE);

    TLVServer *tlvServer = (TLVServer *)ctx;
    bufferevent_setcb(bev, server_read_cb_v6, NULL, server_event_cb_v6, tlvServer);
    bufferevent_enable(bev, EV_READ|EV_WRITE);

    if (tlvServer->accept_conn_handler_v6 != NULL) {
        tlvServer->accept_conn_handler_v6(bev, address, socklen, NULL);
    }
}

static void
accept_error_cb_v6(struct evconnlistener *listener, void *ctx)
{
    struct event_base *base = evconnlistener_get_base(listener);
    int err = EVUTIL_SOCKET_ERROR();
    log_error("v6 Got an error %d (%s) on the listener. "
              "Shutting down.\n", err, evutil_socket_error_to_string(err));
    TLVServer *tlvServer = (TLVServer *)ctx;
    if (tlvServer->accept_error_handler_v6) {
        log_debug("tlvServer->acceptErrorHandler_v6 p: %p", tlvServer->accept_error_handler_v6);
        tlvServer->accept_error_handler_v6(NULL);
    }
    event_base_loopexit(base, NULL);
}

TLVServer* tlvserver_new_with_conf(TLVServerConf *tlvServerConf)
{
    TLVServer *tlvServer = malloc(sizeof(TLVServer));
    memset(tlvServer, 0, sizeof(TLVServer));

    tlvServer->conf = tlvServerConf;

    int port = tlvServerConf->port;

    if (port<=0 || port>65535) {
        perror("Invalid server port");
        return NULL;
    }

    struct event_base *base;
    base = event_base_new();
    if (!base) {
        log_error("Couldn't open event base");
        return NULL;
    }
    tlvServer->base = base;

    struct evconnlistener *listener;
    struct evconnlistener *listener_v6;

    if (tlvServerConf->inet_type & TLV_INET_4) {
        struct sockaddr_in sin;
        /* Clear the sockaddr before using it, in case there are extra
         * platform-specific fields that can mess us up. */
        memset(&sin, 0, sizeof(sin));
        /* This is an INET address */
        sin.sin_family = AF_INET;
        /* Listen on 0.0.0.0 */
        sin.sin_addr.s_addr = inet_addr(tlvServerConf->inet_addr);
        /* Listen on the given port. */
        sin.sin_port = htons(port);

        listener = evconnlistener_new_bind(base, accept_conn_cb, tlvServer,
                                           LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1,
                                           (struct sockaddr*)&sin, sizeof(sin));
        if (!listener) {
            perror("Couldn't create listener");
            return NULL;
        }

        int fd = evconnlistener_get_fd(listener);
        if (evutil_make_socket_nonblocking(fd) != 0) {
            perror("couldn't make socket nonblocking");
            goto v4err;
        }
        tlvServer->listener = listener;
        evconnlistener_set_error_cb(listener, accept_error_cb);
    }

    if (tlvServerConf->inet_type & TLV_INET_6) { // 如果没有开启IPV6，后面的不执行
        log_debug("server enable ipv6");
        struct sockaddr_in6 sin6;
        memset(&sin6, 0, sizeof(sin6));
        sin6.sin6_family = AF_INET6;
        sin6.sin6_port = htons(port);
        inet_pton(AF_INET6, tlvServerConf->inet_addr6, &sin6.sin6_addr);

        listener_v6 = evconnlistener_new_bind(base, accept_conn_cb_v6, tlvServer, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE | LEV_OPT_BIND_IPV6ONLY,
                                              -1, (struct sockaddr*) &sin6, sizeof(sin6));
        if (!listener_v6) {
            perror("Couldn't create ipv6 listener\n");
            goto v46err;
        }

        int fdv6 = evconnlistener_get_fd(listener_v6);
        if (evutil_make_socket_nonblocking(fdv6) != 0) {
            perror("couldn't make v6 socket nonblocking\n");
            goto v46err;
        }
        tlvServer->listener_v6 = listener_v6;
        evconnlistener_set_error_cb(listener_v6, accept_error_cb_v6);
    }

    return tlvServer;

v4err:
    evconnlistener_free(listener);
    event_base_free(base);
    return NULL;

v46err:
    evconnlistener_free(listener_v6);
    goto v4err;

}


int tlvserver_start(TLVServer *tlvServer)
{
    if (!tlvServer->base) {
        log_error("tlvServer->base is NULL");
        return -1;
    }
    return event_base_dispatch(tlvServer->base);
}
