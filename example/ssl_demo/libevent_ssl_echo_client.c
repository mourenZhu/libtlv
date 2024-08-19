#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/bufferevent_ssl.h>
#include <event2/buffer.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#include <arpa/inet.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#define BUFFER_LEN 1024

static void
client_event_cb(struct bufferevent *bev, short events, void *ctx) {
    if (events & BEV_EVENT_CONNECTED) {
    } else if (events & BEV_EVENT_ERROR) {
        bufferevent_free(bev);
    } else if (events & BEV_EVENT_EOF) {
        bufferevent_free(bev);
    }
}

static void
client_read_cb(struct bufferevent *bev, void *ctx) {
    struct evbuffer *input = bufferevent_get_input(bev);
    char buffer[1024] = {0};
    bufferevent_read(bev, buffer, sizeof(buffer));
    printf("%s", buffer);
}


static SSL_CTX *
evssl_init(void)
{
    SSL_CTX  *client_ctx;

    /* Initialize the OpenSSL library */
    SSL_load_error_strings();
    SSL_library_init();
    /* We MUST have entropy, or else there's no point to crypto. */
    if (!RAND_poll())
        return NULL;

    client_ctx = SSL_CTX_new(SSLv23_client_method());

//    SSL_CTX_set_options(client_ctx, SSL_OP_NO_SSLv2);

    return client_ctx;
}

void *client_input_thread(void *arg);

int
main(int argc, char **argv)
{
    SSL_CTX *ssl_ctx;
    struct event_base *base;
    SSL *ssl;

    ssl_ctx = evssl_init();
    ssl = SSL_new(ssl_ctx);

    int port = 9999;

    if (argc > 1) {
        port = atoi(argv[1]);
    }
    if (port<=0 || port>65535) {
        puts("Invalid port");
        return 1;
    }

    base = event_base_new();
    if (!base) {
        puts("Couldn't open event base");
        return 1;
    }

    struct bufferevent *bev = bufferevent_openssl_socket_new(base, -1, ssl, BUFFEREVENT_SSL_OPEN, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, client_read_cb, NULL, client_event_cb, NULL);

    if (bufferevent_socket_connect_hostname(
            bev, NULL, AF_INET, "127.0.0.1", 9999) < 0) {
        bufferevent_free(bev);
        event_base_free(base);
        return 1;
    }

    bufferevent_enable(bev, EV_READ | EV_WRITE);

    pthread_t client_input_thread_t;
    pthread_create(&client_input_thread_t, NULL, client_input_thread, bev);
    pthread_detach(client_input_thread_t);

    printf("event_base_dispatch\n");
    event_base_dispatch(base);

    return 0;
}

void *client_input_thread(void *arg)
{
    struct bufferevent *bev = (struct bufferevent *) arg;

    char buffer[BUFFER_LEN] = {0};
    printf("please enter: ");
    while (1) {
        fgets(buffer, BUFFER_LEN, STDIN_FILENO);
        bufferevent_write(bev, buffer, strlen(buffer));

    }
    return NULL;
}