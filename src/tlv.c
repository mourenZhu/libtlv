#include "tlv/tlv.h"
#include <stdlib.h>
#include <string.h>

TLV* tlv_new()
{
    TLV *tlv = malloc(sizeof(TLV));
    tlv->type = NULL;
    tlv->length = 0;
    tlv->value = NULL;
    return tlv;
}

void tlv_free(TLV **tlv)
{
    if (tlv != NULL && *tlv != NULL) {
        SAFE_FREE((*tlv)->type);
        SAFE_FREE((*tlv)->value);
        SAFE_FREE(*tlv);
    }
}

void tlv_reset(TLV *tlv)
{
    if (tlv != NULL) {
        SAFE_FREE(tlv->type);
        SAFE_FREE(tlv->value);
        tlv->length = 0;
    }
}

TLV* tlv_new_ref(char *type, void *val, size_t len)
{
    TLV *tlv = tlv_new();
    tlv->type = type;
    tlv->length = len;
    tlv->value = val;
    return tlv;
}

TLV* tlv_new_copy(const char *type, const void *val, size_t len)
{
    TLV *tlv = tlv_new();
    tlv->type = malloc(strlen(type) + 1);
    strcpy(tlv->type, type);
    void *value = NULL;
    if (len > 0) {
        value = malloc(len);
        memcpy(value, val, len);
    }
    tlv->value = value;
    tlv->length = len;
    return tlv;
}

