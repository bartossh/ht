#include "ht.h"
#include <stdlib.h>

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
