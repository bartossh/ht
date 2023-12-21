#ifndef HT_H
#define HT_H

/// HT_hash hashes the nullable string.
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
/// str - is a string to be hashed
unsigned long
HT_hash(unsigned char *str);

#endif
