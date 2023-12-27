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

Entities *newEntity(char *key, void *value) {
    Entity *en = malloc(sizeof(Entity*));
    en->key = key;
    en->value = value;

    Entities *ens = malloc(sizeof(Entities*));
    ens->cap = 2;
    ens->arr = calloc(ens->cap, sizeof(Entity**));
    ens->arr[0] = en;
    ens->len = 1;

    return  ens;
}

int appendEntity(Entities *ens, char *key, void *value) {
    for (size_t i = 0; i < ens->len; i++) {
        if (strcmp(key, ens->arr[i]->key) == 0) {
            ens->arr[i]->value = value;
            return 0;
        }
    }
    if (ens->len == ens->cap) {
        ens->cap *= 2;
        ens->arr = realloc(ens->arr, ens->cap*sizeof(Entity**));
    }
    Entity *en = malloc(sizeof(Entity*));
    en->key = key;
    en->value = value;
    
    ens->arr[ens->len] = en;
    ens->len++;

    return 0;
}

HT *HT_new(size_t cap, HT_HashFunction f) {
    HT *ht = malloc(sizeof(HT*));
    ht->hash_function = f;
    ht->cap = cap;
    ht->len = 0;
    ht->table = calloc(cap, sizeof(Entities**));
    return ht;
}

int HT_insert(HT *ht, char *key, void *value) {
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
    return appendEntity(ens, key, value);
}

int HT_read(HT *ht, char *key, void **value) {
    return 0;
}

int HT_delete(HT *ht, char *key, void **value) {
    return 0;
}

int HT_free(HT *ht) {
    return 0;
}
