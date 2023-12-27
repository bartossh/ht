#ifndef HT_H
#define HT_H

#include <stddef.h>

const int HT_ErrCannotInsert = 1;
const int HT_ErrCannotUpdate = 2;
const int HT_ErrDoNotExists = 3;
const int HT_ErrCannotDelete = 4;

typedef unsigned long (*HT_HashFunction)(char *);

/// HT_HashDJB2 hashes the nullable string.
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
HT_HashDJB2(unsigned char *str);


/// HT_HashSDBM hashes the nullable string.
/// It uses sdbm algorithm that was created for sdbm (a public-domain reimplementation of ndbm)
/// database library. It was found to do well in scrambling bits.
///
/// str - is a string to be hashed.
unsigned long
HT_HashSDBM(unsigned char *str);


/// HT_HashLL hashes the nullable string.
/// It uses loss loss algorithm.
/// It is a terrible hashing algorithm, 
/// and it could have been much better without sacrificing its "extreme simplicity."
///
/// str - is a string to be hashed.
unsigned long
HT_HashLL(unsigned char *str);

typedef struct entity {
    char *key;
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
    HT_HashFunction hash_function;
    Entities **table;
    size_t len;
    size_t cap;
} HT;

/// HT_new creates a new has table of initial size.
/// Returns pointer to underlining hash table.
///
/// cap - capacity of the hash table.
/// f - hashing function pointer.
HT *HT_new(size_t cap, HT_HashFunction f);

/// HT_insert inserts a value pointer with given key to the hash table.
/// Value is updated if exists in the hash table.
/// Returns 0 if insert succeeded or error value otherwise.
///
/// ht - pointer to the hash table.
/// key - char* nullable string that represents the key.
/// value - a void pointer to the underlining entity.
int HT_insert(HT *ht, char *key, void *value);

/// HT_read reads a value to a pointer with given key from the hash table if value exists.
/// Returns 0 if update succeeded or error value otherwise.
/// Caller responsibility is to properly cast the pointer to the expected type.
/// Do not free the value memory until the entity is deleted from the hash table.
///
/// ht - pointer to the hash table.
/// key - char* nullable string that represents the key.
/// value - a void pointer to a pointer value, will be updated with the pointer from the hash table.
int HT_read(HT *ht, char *key, void **value);


/// HT_delete deletes a value from the hash table.
/// Returns 0 if deleting succeeded or error value otherwise.
/// Caller responsibility is to free the memory allocated for the value.
///
/// ht - pointer to the hash table.
/// key - char* nullable string that represents the key.
/// value - a void pointer to a pointer value, that was deleted from the hash table,
///         should be freed by the caller.
int HT_delete(HT *ht, char *key, void **value);

/// HT_free frees the memory allocated for the hash table.
/// It is a caller responsibility to free underlining values.
///
/// ht - pointer to the hash table.
int HT_free(HT *ht);

#endif
