#ifndef ht_H
#define ht_H

#include <stddef.h>

#define MinSize 10000
#define MaxSize 65535

#define ht_ErrCannotInsert 1;
#define ht_ErrDoNotExists 2;

typedef unsigned long (*ht_HashFunction)(unsigned char *);

/// ht_HashDJB2 hashes the nullable string.
/// It uses djb2 hashing algorithm 
/// Written by Daniel J. Bernstein (also known as djb), 
/// this simple hash function dates back to 1991.
///
/// Multiplying hash by 33 could be performed by doing hash * 33. 
/// However, the function instead uses (hash << 5) + hash bit 
/// shifts which is on many CPUs a faster way to perform this operation. 
/// hash << 5 “shifts” the bits to the left by 5 spaces, 
/// multiplying the number by 32 (2^5) and + hash adds another value of hash, 
/// turning this into multiplying by 33.
///
/// str - is a string to be hashed.
unsigned long
ht_HashDJB2(unsigned char *str);


/// ht_HashSDBM hashes the nullable string.
/// It uses sdbm algorithm that was created for sdbm (a public-domain reimplementation of ndbm)
/// database library. It was found to do well in scrambling bits.
///
/// str - is a string to be hashed.
unsigned long
ht_HashSDBM(unsigned char *str);


/// ht_HashLL hashes the nullable string.
/// It uses loss loss algorithm.
/// It is a terrible hashing algorithm, 
/// and it could have been much better without sacrificing its "extreme simplicity."
///
/// str - is a string to be hashed.
unsigned long
ht_HashLL(unsigned char *str);

/// Iterator keeps track of the hash map iteration.
typedef struct iterator {
    size_t hash_table_idx;
    size_t arr_idx;
} Iterator;


/// Entity contains of a key and value of the thing stored in a hash map.
typedef struct entity {
    unsigned char *key;
    void *value;
} Entity;

typedef struct entities {
    Entity **arr;
    size_t len;
    size_t cap;
} Entities;

/// HT is a hash table entity of key value pairs.
/// Key in a char* nullable string.
/// Value is a void pointer. 
/// The caller responsibility is to manage memory allocated to store the value.
typedef struct {
    ht_HashFunction hash_function;
    Entities **table;
    size_t len;
    size_t cap;
} HT;

/// ht_new creates a new has table of initial size.
/// Returns pointer to underlining hash table.
///
/// cap - capacity of the hash table.
/// f - hashing function pointer.
HT ht_new(size_t cap, ht_HashFunction f);

/// ht_insert inserts a value pointer with given key to the hash table.
/// Value is updated if exists in the hash table.
/// Returns 0 if insert succeeded or error value otherwise.
///
/// ht - pointer to the hash table.
/// key - char* nullable string that represents the key.
/// value - a void pointer to the underlining entity.
int ht_insert(HT *ht, unsigned char *key, void *value);

/// ht_read reads a value to a pointer with given key from the hash table if value exists.
/// Returns pointer to the value or NULL otherwise;
/// Caller responsibility is to properly cast the pointer to the expected type.
/// Do not free the value memory until the entity is deleted from the hash table.
///
/// ht - pointer to the hash table.
/// key - char* nullable string that represents the key.
void *ht_read(HT *ht, unsigned char *key);


/// ht_delete deletes a value from the hash table.
/// Returns pointer to the value or NULL otherwise;
/// Caller responsibility is to free the memory allocated for the value.
///
/// ht - pointer to the hash table.
/// key - char* nullable string that represents the key.
void *ht_delete(HT *ht, unsigned char *key);

/// ht_newIterator creates new iterator.
Iterator ht_newIterator(void);

/// ht_next allows to iterate over key values pairs.
/// Returns pointer to the next Entity of key value pair or NULL if iterator is exhausted.
///
/// ht - pointer to the hash table.
/// it - pointer to new iterator.
Entity *ht_next(HT *ht, Iterator *it);

/// ht_free frees the memory allocated for the hash table.
/// It is a caller responsibility to free underlining values.
///
/// ht - pointer to the hash table.
void ht_free(HT *ht);

#endif
