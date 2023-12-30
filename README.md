# HT

Hash table in C.

## Genesis

This repository happened as a need for a hash table that offers simple ergonomics to use in other projects.

It is not the perfect or the most efficient one. If you still want to use it look in to the Usage Examples.

## Testing

To test run `make test`.
To test with memcheck run `make memcheck`.

## Usage Examples

Below example consists:

- Creation of the hash table with function `HT ht_new(size_t cap, ht_HashFunction f);`.
- Insertion to the hash table with function `int ht_insert(HT *ht, unsigned char *key, void *value);`.
- Reading from the hash table with function `void *ht_read(HT *ht, unsigned char *key);`.
- Deleting from the hash table with function `void *ht_delete(HT *ht, unsigned char *key);`.
- Iteration of the hash table with function `HT ht_next(HT *ht, Iterator *it);`.
- Freeing the memory after hash table is not needed anymore with function `void ht_free(HT ht);`.


```c
#include <stddef.h>
#include "./path_to_ht_root/src/ht.h"

#define HashTableSize 10000

int main(void) {
    // Create Hash Table example:
    HT ht = ht_new(HashTableSize, ht_HashDJB2);

    char keys[3][4] = {"1234", "next", "last"};
   
    // Insertion example:
    for (size_t i = 1; i < 3; i++) {
        // Allocating int in the heap as a example of value stored as a pointer,
        size_t *v = malloc(sizeof(size_t));
        memcpy(v, &i, sizeof(size_t));
        // till here.
        
        int result = ht_insert(&ht, (unsigned char*)keys[i], (void*)v);
        if (result != 0) {
            printf("insertion failed\n");
            return result;
        }
    }

    // Reading example:
    for (size_t i = 1; i < 3; i++) { 
        coid *value = ht_read(&ht, (unsigned char*)keys[i];
        if (!value) {
            printf("reading failed\n");
            return 1;
        }
        size_t v = (size_t)(*value); // Caller responsibility is to properly cast variable.
        printf("red value [ %lu ]\n", v);
    }
    
    // Deleting example:
    for (size_t i = 1; i < 3; i++) { 
        coid *value = ht_delte(&ht, (unsigned char*)keys[i];
        if (!value) {
            printf("deleting failed\n");
            return 1;
        }
        size_t v = (size_t)(*value); // Caller responsibility is to properly cast variable.
        printf("deleted value [ %lu ]\n", v);
        free(value); // Caller responsibility is to free value after deletion and usage.
    }
    

    // Iterating example:
    Iterator it = ht_newIterator();
    while (true) {
        Entity *en = ht_next(&ht, &it);
        if (!en) {
            break;
        }
        size_t *value = (size_t*)(en->value);
        printf("next value [ %lu ]\n", *value);
    }

    // Free Memory used by a Hash Table example:
    ht_free(ht); 

    return 0;

}
```


## Benchmarks

### Hardware and OS

- Chip: Apple M2
- Memory: 24 GB
- System Firmware Version: 10151.61.4

### Results

- ht_HashDJB2 calculating 48000000 hashes took 1.182072_sec.
- ht_HashSDBM calculating 48000000 hashes took 1.221105_sec.
- ht_HashLL calculating 48000000 hashes took 1.087453_sec.
- Inserting 4800 entities with hash function dbj2 took 0.001356_sec.
- Inserting 4800 entities with hash function sbdm took 0.001400_sec.
- Inserting 4800 entities with hash function loss loss took 0.001657_sec.
- Reading 4800 entities with hash function dbj2 took 0.000265_sec.
- Reading 4800 entities with hash function sbdm took 0.000283_sec.
- Reading 4800 entities with hash function loss loss took 0.000979_sec.
- Deleting 4800 entities with hash function dbj2 took 0.000460_sec.
- Deleting 4800 entities with hash function sbdm took 0.000496_sec.
- Deleting 4800 entities with hash function loss loss took 0.000945_sec.
- Iterating over 4800 entities with hash function dbj2 took 0.000314_sec.
- Iterating over 4800 entities with hash function sbdm took 0.000319_sec.
- Iterating over 4800 entities with hash function loss loss took 0.000297_sec.

