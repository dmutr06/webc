#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define __HT_INIT_CAP__ 64 

#define __HT_FACTOR__ .5

typedef enum {
    HT_BUCKET_STATE_EMPTY,
    HT_BUCKET_STATE_OCCUPIED,
    HT_BUCKET_STATE_DELETED,
} __HtBucketState;

static inline size_t __ht_hash(const char *key) {
    if (!key) return 0;
    size_t total = 0;
    while (*key) {
        total += *key;
        key += 1;
    }

    return total;
}


static inline char *__ht_get_internal(
    char *buckets,
    size_t size,
    size_t cap,
    size_t val_offset,
    size_t state_offset,
    size_t bucket_size,
    const char *key
) {
    if (size == 0) return NULL;
    size_t idx = __ht_hash(key);

    for (size_t i = 0; i < cap; ++i) {
        char *cur = buckets + ((i + idx) & (cap - 1)) * bucket_size;
        __HtBucketState *state =  (__HtBucketState *) (cur + state_offset);
        char *val = cur + val_offset;
        char *cur_key = (char *) *(char **) cur;

        switch (*state) {
            case HT_BUCKET_STATE_EMPTY: {
                break;
            }
            case HT_BUCKET_STATE_OCCUPIED: {
                if (strcmp(cur_key, key) == 0)
                    return val;

                break;
            }
            case HT_BUCKET_STATE_DELETED: {
                if (strcmp(cur_key, key) == 0)
                    return NULL;

                break;
            }
        }
    }

    return NULL;
}

#define Bucket(Val) \
struct { \
    char *key; \
    Val val; \
    __HtBucketState state; \
}

#define HashTable(Val) \
struct { \
    Bucket(Val) *buckets; \
    size_t cap; \
    size_t size; \
    void (*deinit)(Val *val); \
}

#define ht_init(ht, deinit_func) do { \
    (ht)->cap = __HT_INIT_CAP__; \
    (ht)->size = 0; \
    (ht)->buckets = malloc(sizeof(*((ht)->buckets)) * __HT_INIT_CAP__); \
    (ht)->deinit = deinit_func; \
    memset((ht)->buckets, 0, (ht)->cap * sizeof(*((ht)->buckets))); \
} while (0)

#define __ht_unsafe_insert(ht, key_, val_) do { \
    size_t idx = __ht_hash(key_); \
    for (size_t i = 0; i < (ht)->cap; ++i) { \
        typeof(((ht)->buckets)) cur = (ht)->buckets + ((i + idx) & ((ht)->cap - 1)); \
        if (cur->state == HT_BUCKET_STATE_DELETED || cur->state == HT_BUCKET_STATE_EMPTY) { \
            if (cur->state == HT_BUCKET_STATE_DELETED) free(cur->key); \
            cur->val = val_; \
            cur->key = malloc(strlen(key_) + 1); \
            strcpy(cur->key, key_); \
            (ht)->size += 1; \
            cur->state = HT_BUCKET_STATE_OCCUPIED; \
            break; \
            \
         } \
        if (strcmp(cur->key, key_) == 0) { \
            cur->val = val_; \
            break; \
        } \
    } \
} while (0)

#define ht_insert(ht, key, val) do { \
    if ((ht)->size >= (ht)->cap * __HT_FACTOR__) ht_resize(ht); \
    __ht_unsafe_insert(ht, key, val); \
} while (0)

#define ht_resize(ht) do { \
    size_t old_cap = (ht)->cap; \
    typeof((ht)->buckets) old_buckets = (ht)->buckets; \
    (ht)->cap *= 2; \
    (ht)->buckets = malloc((ht)->cap * sizeof(*((ht)->buckets))); \
    memset((ht)->buckets, 0, (ht)->cap * sizeof(*((ht)->buckets))); \
    (ht)->size = 0; \
    for (size_t i = 0; i < old_cap; ++i) { \
        typeof(*((ht)->buckets)) *cur = old_buckets + i; \
         if (cur->state == HT_BUCKET_STATE_DELETED) free(cur->key); \
         else if (cur->state == HT_BUCKET_STATE_OCCUPIED) { \
            const char *cur_key = cur->key; \
            const typeof((ht)->buckets->val) *cur_val = &cur->val; \
            __ht_unsafe_insert(ht, cur_key, *cur_val); \
         } \
    } \
} while (0) 


#define ht_get(ht, key_) (typeof((ht)->buckets->val) *) \
__ht_get_internal( \
    (char *) (ht)->buckets, \
    (ht)->size, (ht)->cap, \
    offsetof(typeof(*((ht)->buckets)), val), \
    offsetof(typeof(*((ht)->buckets)), state), \
    sizeof(*((ht)->buckets)), \
    key_ \
)

#define ht_remove(ht, key_) do { \
    size_t idx = __ht_hash(key_); \
    for (size_t i = 0; i < (ht)->cap; ++i) { \
        typeof(((ht)->buckets)) cur = (ht)->buckets + (i + idx) & ((ht)->cap - 1); \
        if (cur->state == HT_BUCKET_STATE_DELETED && strcmp(cur->key, key_) == 0) break; \
        else continue; \
        if (cur->state == HT_BUCKET_STATE_EMPTY) { \
            continue; \
        } \
        if (strcmp(cur->key, key_) == 0) { \
            (ht)->size -= 1; \
            cur->state = HT_BUCKET_STATE_DELETED; \
            break; \
        } \
    } \
} while (0)

#define ht_deinit(ht) do { \
    for (size_t i = 0; i < (ht)->cap; ++i) { \
        typeof((ht)->buckets) cur = (ht)->buckets + i; \
        switch (cur->state) { \
            case HT_BUCKET_STATE_EMPTY: break; \
            case HT_BUCKET_STATE_DELETED: { \
                free(cur->key); \
                break; \
            } \
            case HT_BUCKET_STATE_OCCUPIED: { \
                free(cur->key); \
                if ((ht)->deinit) (ht)->deinit(&cur->val); \
                break; \
            } \
        } \
    } \
    free((ht)->buckets); \
} while (0)

#endif

