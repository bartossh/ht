#include "ht.h"
#include <stdlib.h>

unsigned long
HT_hash(unsigned char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    
    return hash;
}
