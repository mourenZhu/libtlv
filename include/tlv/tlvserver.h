#ifndef TLVSERVER_H
#define TLVSERVER_H
#include <stdlib.h>
#include <event2/listener.h>
#include <netinet/in.h>
#include "tlvevent.h"

typedef struct tlvserver_conf
{
    char inet_addr[INET_ADDRSTRLEN];
    char inet_addr6[INET6_ADDRSTRLEN];
    u_int16_t port;
    u_int8_t inet_type;
} TLVServerConf;

typedef struct tlvserver
{
    TLVServerConf *conf;
    struct event_base *base;
    struct evconnlistener *listener;
    tlv_handler tlv_handler;
    et_accept_conn_handler accept_conn_handler;
    et_accept_error_handler accept_error_handler;
    et_event_handler event_handler;
    struct evconnlistener *listener_v6;
    tlv_handler tlv_handler_v6;
    et_accept_conn_handler accept_conn_handler_v6;
    et_accept_error_handler accept_error_handler_v6;
    et_event_handler event_handler_v6;
}TLVServer;

/**
 * init
 * @return
 */
int tlvsconf_init(TLVServerConf *tlvServerConf, const char *inet_addr, const char *inet_addr6, u_int16_t port, u_int8_t inet_type);

/**
 *
 * @param tlvServerConf
 * @return
 */
TLVServer* tlvserver_new_with_conf(TLVServerConf *tlvServerConf);

/**
 *
 * @param tlvServer
 * @return 0 if successful, -1 if an error occurred, or 1 if we exited because no events were pending or active.
 */
int tlvserver_start(TLVServer *tlvServer);

/**
 *
 * @param tlvServConf
 * @return 0 if successful, or -1 if an error occurred
 */
int tlvserver_exit(TLVServer *tlvServer);


#endif