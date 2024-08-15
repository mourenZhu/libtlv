#ifndef TLVEVENT_H
#define TLVEVENT_H

#include "tlv.h"

/**
 * 查看 <event2/buffer.h>中的 enum evbuffer_eol_style 定义
 * libtlv默认使用的格式为EVBUFFER_EOL_CRLF，它的值为1
 */
#define TLV_TYPE_EOL_STYLE 1
#define TLV_TYPE_EOL_CHAR \r\n
#define TLV_TYPE_EOL_LENGTH 2
#define TLV_HOSTNAME_MAX_LENGTH 253


#define TLV_INET_4 1
#define TLV_INET_6 2

/**
 * @param tlv 就是一个tlv
 * @param ctx 还没想好放什么，暂时是NULL
 */
typedef void (*tlv_handler) (TLV *tlv, struct bufferevent *bev, void *ctx);

/**
 * @param ctx 还没想好放什么，暂时是NULL
 */
typedef void (*et_accept_conn_handler) (struct bufferevent *bev, struct sockaddr *address, int socklen, void *ctx);

/**
 * @param ctx 还没想好放什么，暂时是NULL
 */
typedef void (*et_accept_error_handler) (void *ctx);

/**
 * @param ctx 还没想好放什么，暂时是NULL
 */
typedef void (*et_event_handler) (struct bufferevent *bev, short events, void *ctx);


/**
 * 通过bufferevent获取一个tlv，如果不能获取一个完整的tlv则返回NULL。返回的TLV需要调用者手动销毁
 * @param bev
 * @return 数据足够则返回一个TLV
 */
TLV *tlv_read_new_with_bufferevent(struct bufferevent *bev);

/**
 * tlv 通过bufferevent发送tlv数据
 *
 * @param bev
 * @param tlv
 * @return 0 if successful, or -1 if an error occurred
 */
int tlv_send(struct bufferevent *bev, TLV *tlv);

/**
 *
 * @param hostname
 * @param port
 * @return
 */
evutil_socket_t
get_tcp_socket_for_host(const char *hostname, ev_uint16_t port);

#endif