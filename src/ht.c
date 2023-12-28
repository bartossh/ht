#include "ht.h"
#include <stdlib.h>
#include <string.h>

unsigned long
HT_HashDJB2(unsigned char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    
    return hash;
}

unsigned long
HT_HashSDBM(unsigned char *str) {
    unsigned long hash = 0;
    int c;

    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

unsigned long
HT_HashLL(unsigned char *str) {
    unsigned int hash = 0;
    int c;

    while ((c = *str++))
        hash += c;

    return hash;
}

static Entities *newEntity(unsigned char *key, void *value) {
    Entity *en = malloc(sizeof(Entity));
    en->key = key;
    en->value = value;

    Entities *ens = malloc(sizeof(Entities));
    ens->cap = 2;
    ens->arr = calloc(ens->cap, sizeof(Entity*));
    ens->arr[0] = en;
    ens->len = 1;

    return ens;
}

static int appendEntity(Entities *ens, unsigned char *key, void *value) {
    for (size_t i = 0; i < ens->len; i++) {
        if (strcmp((const char*)(key), (const char*)(ens->arr[i]->key)) == 0) {
            ens->arr[i]->value = value;
            return 0;
        }
    }
    if (ens->len == ens->cap) {
        ens->cap *= 2;
        ens->arr = realloc(ens->arr, ens->cap*sizeof(Entity*));
    }
    Entity *en = malloc(sizeof(Entity));
    en->key = key;
    en->value = value;
    
    ens->arr[ens->len] = en;
    ens->len++;

    return 0;
}

static void *getEntityValue(Entities *ens, unsigned char *key) {
    for (size_t i = 0; i < ens->len; i++) {
        if (strcmp((const char*)(key), (const char*)(ens->arr[i]->key)) == 0) {
            return ens->arr[i]->value;
        }
    }
    return NULL;
}

static void *deleteEntitytValue(Entities *ens, unsigned char *key) {
    for (size_t i = 0; i < ens->len; i++) {
        if (strcmp((const char*)(key), (const char*)(ens->arr[i]->key)) == 0) {
            void *value = ens->arr[i]->value;
            free(ens->arr[i]);
            ens->arr[i] = ens->arr[--ens->len];
            return value;
        }
    }
    return NULL;
}

static void freeEntities(Entities *ens) {
    if (!ens) {
        return;
    }
    for (size_t i = 0; i < ens->len; i++) {
        free(ens->arr[i]);
    }
    free(ens->arr);
    free(ens);
    return;
}

HT HT_new(size_t cap, HT_HashFunction f) {
    if (cap < MinSize) {
        cap = MinSize;
    }
    if (cap > MaxSize) {
        cap = MaxSize;
    }
    HT ht = {
        .hash_function = f,
        .cap = cap,
        .len = 0,
        .table = calloc(cap, sizeof(Entities**)),
    };
    return ht;
}

int HT_insert(HT *ht, unsigned char *key, void *value) {
    if (!ht) {
        return HT_ErrDoNotExists;
    }
    unsigned long h = ht->hash_function(key);
    size_t idx = (size_t)(h)%ht->cap;
    if (!ht->table[idx]) {
        Entities *ens = newEntity(key, value);
        ht->table[idx] = ens;
        return 0;
    }
    Entities *ens = ht->table[idx];
    int result = appendEntity(ens, key, value);
    if (result != 0) {
        return result;
    }
    ht->len++;
    return 0;
}

void *HT_read(HT *ht, unsigned char *key) {
    if (!ht) {
        return NULL;
    }

    unsigned long h = ht->hash_function(key);
    size_t idx = (size_t)(h)%ht->cap;
    Entities *ens = ht->table[idx];
    if (!ens) {
        return NULL;
    }

    void *candidate = getEntityValue(ens, key);
    if (!candidate) {
        return NULL;
    }
    return candidate;
}

void *HT_delete(HT *ht, unsigned char *key) {
    if (!ht) {
        return NULL;
    }

    unsigned long h = ht->hash_function(key);
    size_t idx = (size_t)(h)%ht->cap;
    Entities *ens = ht->table[idx];
    if (!ens) {
        return NULL;
    }

    void *candidate = deleteEntitytValue(ens, key);
    if (!candidate) {
        return NULL;
    }
    ht->len--;

    return candidate;
}

Iterator HT_newIterator(void) {
    Iterator it = {
        .hash_table_idx = 0,
        .arr_idx = 0
    };
    return it;
}

Entity *HT_next(HT *ht, Iterator *it) {
    if (!ht || !it) {
        return NULL;
    }
    size_t i = it->hash_table_idx;
    size_t j = it->arr_idx;
    for (; i < ht->cap; i++) {
        Entities *ens = ht->table[i];
        if (!ens) {
            j = 0;
            continue;
        }
        if (j >= ens->len) {
            j = 0;
            continue;
        }
        Entity *en = ens->arr[j];
        it->hash_table_idx = i;
        it->arr_idx = j+1;
        return en;
    }

    it->hash_table_idx = ht->cap;
    it->arr_idx = 0;
    return NULL;
}

void HT_free(HT *ht) {
    if (!ht) {
        return;
    }
    if (!ht->table) {
        return;
    }
    for (size_t i = 0; i < ht->cap; i++) {
        Entities *ens = ht->table[i];
        if (ens) {
            freeEntities(ens);
        }
    }
    free(ht->table);
    ht->cap = 0;
    ht->len = 0;
    ht->table = NULL;
    return;
}
