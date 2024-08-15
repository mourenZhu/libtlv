#include <stdio.h>
#include <string.h>
#include <tlv//tlvserver.h>

void server_tlv_handler(TLV *tlv, struct bufferevent *bev, void *ctx)
{
    printf("server tlv type: %s, length: %ld, value: %s\n", tlv->type, tlv->length, (char *)tlv->value);
    char msg[128] = "hello tlv!";
    TLV *send_tlv = tlv_new_copy("hello", msg, strlen(msg) + 1);
    tlv_send(bev, send_tlv);
    tlv_free(&send_tlv);
}

int main(int argc, char *argv[])
{
    printf("server start\n");
    TLVServerConf tlvServerConf;
    tlvsconf_init(&tlvServerConf, "0.0.0.0", "", 10816, TLV_INET_4);
    TLVServer *tlvServer = tlvserver_new_with_conf(&tlvServerConf);
    tlvServer->tlv_handler = server_tlv_handler;
    tlvserver_start(tlvServer);
    return 0;
}