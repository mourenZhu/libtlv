#include "tlv/tlvevent.h"
#include <string.h>
#include <stdlib.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <assert.h>
#include "tlv/log.h"

/**
 * 检查bev中是否有一个完整的tlv数据
 *
 * @param bev
 * @return 0有完整数据，-1没有完整数据
 */
static int bufferevent_check_has_tlv(struct bufferevent *bev)
{
    struct evbuffer *input = bufferevent_get_input(bev);
    size_t inbuf_length = evbuffer_get_length(input);

    size_t eol_len_out = 0;
    struct evbuffer_ptr evbuffer_ptr = evbuffer_search_eol(input, NULL, &eol_len_out, TLV_TYPE_EOL_STYLE);
    if (evbuffer_ptr.pos == -1) { // 检查是否有一个完整的type
        return -1;
    }
    log_trace("evbuffer_ptr.pos = %ld, eol_len_out = %ld", evbuffer_ptr.pos, eol_len_out);
    if (inbuf_length < evbuffer_ptr.pos + TLV_TYPE_EOL_LENGTH + TLV_LENGTH_LENGTH) { // 检查是否已收到完整的type + length
        return -1;
    }
    size_t tlv_value_length;
    struct evbuffer_ptr pos;
    evbuffer_ptr_set(input, &pos, evbuffer_ptr.pos + TLV_TYPE_EOL_LENGTH, EVBUFFER_PTR_SET);
    evbuffer_copyout_from(input, &pos, &tlv_value_length, TLV_LENGTH_LENGTH);
    if (inbuf_length < evbuffer_ptr.pos + TLV_TYPE_EOL_LENGTH + TLV_LENGTH_LENGTH + tlv_value_length) {
        return -1;
    }
    return 0;
}

TLV *tlv_read_new_with_bufferevent(struct bufferevent *bev)
{
    struct evbuffer *input = bufferevent_get_input(bev);

    int fd = bufferevent_getfd(bev);
    if (bufferevent_check_has_tlv(bev) < 0) {
        return NULL;
    }
    TLV *tlv = tlv_new();
    tlv->type = evbuffer_readln(input, NULL, TLV_TYPE_EOL_STYLE);
    log_debug("fd: %d, type: %s", fd, tlv->type);
    bufferevent_read(bev, &tlv->length, TLV_LENGTH_LENGTH);
    log_debug("fd: %d, tlv->length: %ld", fd, tlv->length);
    if (tlv->length > 0) {
        tlv->value = malloc(tlv->length);
        memset(tlv->value, 0, tlv->length);
        bufferevent_read(bev, tlv->value, tlv->length);
    }

    return tlv;
}


int tlv_send(struct bufferevent *bev, TLV *tlv)
{
    size_t type_len = strlen(tlv->type) + 2;
    size_t len_len = sizeof(tlv->length);
    size_t val_len = tlv->length;
    size_t total_len = type_len + len_len + val_len;
    char *tlvdata = malloc(total_len);
    char type_char[type_len];
    sprintf(type_char, "%s\r\n", tlv->type); // 添加\r\n
    memcpy(tlvdata, type_char, type_len); // 添加完整type
    memcpy(tlvdata+type_len, &tlv->length, sizeof(tlv->length)); // 添加length
    if (tlv->length > 0) {
        memcpy(tlvdata+type_len+len_len, tlv->value, tlv->length);
    }
    int ret;
    ret = bufferevent_write(bev, tlvdata, total_len);
    free(tlvdata);
    return ret;
}

evutil_socket_t
get_tcp_socket_for_host(const char *hostname, ev_uint16_t port)
{
    char port_buf[6];
    struct evutil_addrinfo hints;
    struct evutil_addrinfo *answer = NULL;
    int err;
    evutil_socket_t sock;

    /* Convert the port to decimal. */
    evutil_snprintf(port_buf, sizeof(port_buf), "%d", (int)port);

    /* Build the hints to tell getaddrinfo how to act. */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; /* v4 or v6 is fine. */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP; /* We want a TCP socket */
    /* Only return addresses we can use. */
    hints.ai_flags = EVUTIL_AI_ADDRCONFIG;

    /* Look up the hostname. */
    err = evutil_getaddrinfo(hostname, port_buf, &hints, &answer);
    if (err != 0) {
        fprintf(stderr, "Error while resolving '%s': %s",
                hostname, evutil_gai_strerror(err));
        return -1;
    }

    /* If there was no error, we should have at least one answer. */
    assert(answer);
    /* Just use the first answer. */
    sock = socket(answer->ai_family,
                  answer->ai_socktype,
                  answer->ai_protocol);
    if (sock < 0)
        return -1;
    if (connect(sock, answer->ai_addr, answer->ai_addrlen)) {
        /* Note that we're doing a blocking connect in this function.
         * If this were nonblocking, we'd need to treat some errors
         * (like EINTR and EAGAIN) specially. */
        EVUTIL_CLOSESOCKET(sock);
        return -1;
    }

    return sock;
}
