#ifndef TLVCLIENT_H
#define TLVCLIENT_H
#include <stdlib.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include "tlv.h"
#include "tlvevent.h"

#define TLV_CLIENTID_LENGTH 128

typedef struct tlvclient_conf
{
    char server_hostname[TLV_HOSTNAME_MAX_LENGTH+1];
    char server_hostname6[TLV_HOSTNAME_MAX_LENGTH+1];
    u_int16_t server_port;
    char clientid[TLV_CLIENTID_LENGTH+1];
    u_int8_t inet_type;
} TLVClientConf;

typedef struct tlvclient
{
    struct event_base *base;
    struct bufferevent *bev;
    TLVClientConf *client_conf;
    tlv_handler tlv_handler;
    et_event_handler event_handler;
} TLVClient;

/**
 * 创建一个结构体，并返回指针
 * @return
 */
TLVClient *tlvclient_new();


/**
 * 销毁
 * @param pTlvClient
 */
void tlvclient_free(TLVClient **pTlvClient);


/**
 * 初始化客户端配置结构体
 * @param cid client id
 * @param hostname server ipv4 hostname
 * @param hostname_v6 server ipv6 hostname
 * @param s_port server port
 * @param inet_type sin_family AF_INET AF_INET6
 * @return 0 正常, -1 异常
 */
int tlvcconf_init(TLVClientConf *tlvClientConf, const char *cid, const char *hostname, const char *hostname_v6,
                  u_int16_t s_port, u_int8_t inet_type);


/**
 * 通过配置创建client
 *
 * @param tlvClientConf
 * @return
 */
TLVClient *tlvclient_new_with_conf(TLVClientConf *tlvClientConf);


/**
 *
 * @param tlvClient
 * @return 0 正常，-1异常
 */
int tlvclient_start(TLVClient *tlvClient);

#endif