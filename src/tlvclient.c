#include "tlv/tlvclient.h"
#include "tlv/tlv.h"
#include <string.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <arpa/inet.h>
#include "tlv/log.h"

TLVClient *tlvclient_new()
{
    TLVClient *tlvClient = malloc(sizeof(TLVClient));
    if (tlvClient) {
        memset(tlvClient, 0, sizeof(TLVClient));
    }
    return tlvClient;
}

void tlvclient_free(TLVClient **pTlvClient)
{
    SAFE_FREE(*pTlvClient);
}



int tlvcconf_init(TLVClientConf *tlvClientConf, const char *cid, const char *hostname, const char *hostname_v6, u_int16_t s_port, u_int8_t inet_type)
{
    memset(tlvClientConf, 0, sizeof(TLVClientConf));
    strcpy(tlvClientConf->clientid, cid);
    strcpy(tlvClientConf->server_hostname, hostname);
    strcpy(tlvClientConf->server_hostname6, hostname_v6);
    tlvClientConf->server_port = s_port;
    tlvClientConf->inet_type = inet_type;
    return 0;
}

static void
client_event_cb(struct bufferevent *bev, short events, void *ctx) {
    TLVClient *tlvClient = (TLVClient *)ctx;
    if (tlvClient->event_handler) {
        tlvClient->event_handler(bev, events, NULL);
    }
    if (events & BEV_EVENT_CONNECTED) {
        log_info("Connected to server\n");
    } else if (events & BEV_EVENT_ERROR) {
        log_info("Error in connection\n");
        bufferevent_free(bev);
    } else if (events & BEV_EVENT_EOF) {
        log_info("Connection closed\n");
        bufferevent_free(bev);
    }
}

static void
client_read_cb(struct bufferevent *bev, void *ctx) {
    struct evbuffer *input = bufferevent_get_input(bev);
    log_debug("read cd buffer length: %ld", evbuffer_get_length(input));
    TLV *tlv = NULL;
    TLVClient *tlvServer = (TLVClient *)ctx;
    while ((tlv = tlv_read_new_with_bufferevent(bev))) {
        if (tlvServer->tlv_handler) {
            tlvServer->tlv_handler(tlv, bev, NULL);
        }
        tlv_free(&tlv);
    }
}

TLVClient *tlvclient_new_with_conf(TLVClientConf *tlvClientConf)
{
    struct event_base *base = event_base_new();
    if (!base) {
        log_error("Could not initialize libevent!");
        return NULL;
    }

    struct bufferevent *bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
    if (!bev) {
        log_error("Error constructing bufferevent!");
        event_base_free(base);
        return NULL;
    }
    TLVClient *tlvClient = tlvclient_new();

    bufferevent_setcb(bev, client_read_cb, NULL, client_event_cb, tlvClient);

    if (tlvClientConf->inet_type & TLV_INET_6) {
        log_debug("client start ipv6 connect");
        if (bufferevent_socket_connect_hostname(
                bev, NULL, AF_INET6, tlvClientConf->server_hostname6, tlvClientConf->server_port) < 0) {
            log_error("IPV6 Error starting connection");
            bufferevent_free(bev);
            event_base_free(base);
            tlvclient_free(&tlvClient);
            return NULL;
        }
    } else {
        log_debug("client start ipv4 connect");
        if (bufferevent_socket_connect_hostname(
                bev, NULL, AF_INET, tlvClientConf->server_hostname, tlvClientConf->server_port) < 0) {
            log_error("Error starting connection");
            bufferevent_free(bev);
            event_base_free(base);
            tlvclient_free(&tlvClient);
            return NULL;
        }
    }
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    tlvClient->client_conf = tlvClientConf;
    tlvClient->base = base;
    tlvClient->bev = bev;
    return tlvClient;
}

int tlvclient_start(TLVClient *tlvClient)
{
    if (!tlvClient->base) {
        log_error("tlvClient->base is NULL");
        return -1;
    }
    event_base_dispatch(tlvClient->base);
    return 0;
}


