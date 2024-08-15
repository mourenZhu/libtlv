#ifndef TLV_H
#define TLV_H

#include <event2/bufferevent.h>

#define SAFE_FREE(ptr) do { \
    if (ptr) {              \
        free(ptr);          \
        ptr = NULL;         \
    }                       \
} while (0)

typedef struct tlv {
    char *type;
    size_t length;
    void *value;
} TLV;

#define TLV_LENGTH_LENGTH sizeof(size_t)


/**
 * init tlv
 *
 * @return NULL if an error occurred
 */
TLV* tlv_new();

/**
 * free tlv
 * @param tlv
 */
void tlv_free(TLV **tlv);

/**
 * 重置tlv结构体内的数据
 * @param tlv
 */
void tlv_reset(TLV *tlv);


/**
 * 构建一个tlv，传过来的值直接引用
 * @param type
 * @param val
 * @param len
 * @return
 */
TLV* tlv_new_ref(char *type, void *val, size_t len);

/**
 *
 * 构建一个tlv，并且传的参数不会直接引用，而是构建一个新的值，然后复制
 * @param type
 * @param val
 * @param len
 * @return
 */
TLV* tlv_new_copy(const char *type, const void *val, size_t len);

#endif