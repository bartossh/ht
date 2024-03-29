#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#include "../test-framework/unity.h"
#include "ht.h"

#define ARR_SIZE 1000*1000

typedef struct {
    char **arr;
    size_t len;
} StrArr_view;

void str_arr_free(StrArr_view sa_v) {
    for (int i = 0; i < ARR_SIZE; i++) {
        if (sa_v.arr[i])
            free(sa_v.arr[i]);
    }

    if (sa_v.arr)
        free(sa_v.arr);
}

StrArr_view read(char *file) {
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    StrArr_view arr_view = {
        .arr = NULL,
        .len = 0
    };

    fp = fopen(file, "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
        return arr_view;
    }

    arr_view.arr = malloc(sizeof(char *)*ARR_SIZE);
    arr_view.len = 0;

    while ((read = getline(&line, &len, fp)) != -1) {
        char *str = malloc(sizeof(char)*len);
        strncpy(str, line, len);
        str[strcspn(str, "\n")] = 0;
        arr_view.arr[arr_view.len] = str;
        arr_view.len++;
    }
 
    fclose(fp);
    if (line)
        free(line);

    return arr_view;
}

typedef struct {
    unsigned long hash;
    int counter;
} Occurance;


typedef struct {
    Occurance *arr;
    size_t len;
} OccuranceArr_view;

void occurance_arr_free(OccuranceArr_view oa_v) {
    if (oa_v.arr)
        free(oa_v.arr);
}

OccuranceArr_view occurance_arr_new(size_t len) {
    Occurance *arr = calloc(len, sizeof(Occurance));
    OccuranceArr_view oa_v = {
        .arr = arr,
        .len = len
    };
    return oa_v;
}

void occurance_arr_append_hash(OccuranceArr_view oa_v, unsigned long hash) {
    size_t pos =  (size_t)((hash)%(unsigned long)oa_v.len);
    Occurance exs = oa_v.arr[pos];
    if (hash != exs.hash) {
        exs.hash = hash;
    }
    exs.counter ++;
    oa_v.arr[pos] = exs;
}

size_t occurance_arr_count_distinct(OccuranceArr_view oa_v) {
    size_t counter = 0;
    for (size_t i = 0; i < oa_v.len; i++) {
        if (oa_v.arr[i].counter == 1 && oa_v.arr[i].hash != 0){
            counter++;
        } else if (oa_v.arr[i].hash != 0) {
            //printf("found hash collision [ 0x%lX ] occurances [ %d ]\n", oa_v.arr[i].hash, oa_v.arr[i].counter);
        }
    }
    return counter;
}

void setUp(void)
{
}

void tearDown(void)
{
}

static void testHashCollisionsDjb2Success(void)
{
    StrArr_view sa_v = read("./test-data/probable-v2-wpa-top4800.txt");

    size_t multiplier = 10*1000; 

    OccuranceArr_view oa_v = occurance_arr_new(sa_v.len*multiplier);

    for (size_t i = 0; i < sa_v.len; i++) {
        unsigned long hs = ht_HashDJB2((unsigned char*)sa_v.arr[i]);
        //printf("New hash from [ %s ] => [ 0x%lX ]\n", sa_v.arr[i], hs);
        occurance_arr_append_hash(oa_v, hs);
    }

    size_t distinct = occurance_arr_count_distinct(oa_v);
    size_t all = sa_v.len;

    str_arr_free(sa_v);
    occurance_arr_free(oa_v);

    TEST_ASSERT_EQUAL(all, distinct);
}

static void testHashCollisionsSdbmSuccess(void)
{
    StrArr_view sa_v = read("./test-data/probable-v2-wpa-top4800.txt");

    size_t multiplier = 10*1000; 

    OccuranceArr_view oa_v = occurance_arr_new(sa_v.len*multiplier);

    for (size_t i = 0; i < sa_v.len; i++) {
        unsigned long hs = ht_HashSDBM((unsigned char*)sa_v.arr[i]);
        //printf("New hash from [ %s ] => [ 0x%lX ]\n", sa_v.arr[i], hs);
        occurance_arr_append_hash(oa_v, hs);
    }

    size_t distinct = occurance_arr_count_distinct(oa_v);
    size_t all = sa_v.len;

    str_arr_free(sa_v);
    occurance_arr_free(oa_v);

    TEST_ASSERT_EQUAL(all, distinct);
}

static void testHashCollisionsLL(void)
{
    StrArr_view sa_v = read("./test-data/probable-v2-wpa-top4800.txt");

    size_t multiplier = 10*1000; 

    OccuranceArr_view oa_v = occurance_arr_new(sa_v.len*multiplier);

    for (size_t i = 0; i < sa_v.len; i++) {
        unsigned long hs = ht_HashLL((unsigned char*)sa_v.arr[i]);
        //printf("New hash from [ %s ] => [ 0x%lX ]\n", sa_v.arr[i], hs);
        occurance_arr_append_hash(oa_v, hs);
    }

    size_t distinct = occurance_arr_count_distinct(oa_v);
    size_t all = sa_v.len;

    str_arr_free(sa_v);
    occurance_arr_free(oa_v);

    TEST_ASSERT_NOT_EQUAL(all, distinct);
}

static void bechHashCollisionsDjb2(void)
{
    const size_t repetitons = 10000;
    StrArr_view sa_v = read("./test-data/probable-v2-wpa-top4800.txt");
    
    struct timeval begin, end;
    gettimeofday(&begin, 0);
    
    size_t counter = 0;
    for (size_t rep = 0; rep < repetitons; rep++) {
        for (size_t i = 0; i < sa_v.len; i++) {
            ht_HashDJB2((unsigned char*)sa_v.arr[i]);
            counter++;
        }
    }

    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds*1e-6;
    printf("ht_HashDJB2 calculating %zu hashes took [ %f_sec ]\n", counter, elapsed);

    TEST_ASSERT_EQUAL(0, 0);
}

static void benchHashCollisionsSdbm(void)
{
    const size_t repetitons = 10000;
    StrArr_view sa_v = read("./test-data/probable-v2-wpa-top4800.txt");
    
    struct timeval begin, end;
    gettimeofday(&begin, 0);
    
    size_t counter = 0;
    for (size_t rep = 0; rep < repetitons; rep++) {
        for (size_t i = 0; i < sa_v.len; i++) {
            ht_HashSDBM((unsigned char*)sa_v.arr[i]);
            counter++;
        }
    }

    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds*1e-6;
    printf("ht_HashSDBM calculating %zu hashes took [ %f_sec ]\n", counter, elapsed);

    TEST_ASSERT_EQUAL(0, 0);
}

static void benchHashCollisionsLL (void)
{
    const size_t repetitons = 10000;
    StrArr_view sa_v = read("./test-data/probable-v2-wpa-top4800.txt");
    
    struct timeval begin, end;
    gettimeofday(&begin, 0);
    
    size_t counter = 0;
    for (size_t rep = 0; rep < repetitons; rep++) {
        for (size_t i = 0; i < sa_v.len; i++) {
            ht_HashLL((unsigned char*)sa_v.arr[i]);
            counter++;
        }
    }

    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds*1e-6;
    printf("ht_HashLL calculating %zu hashes took [ %f_sec ]\n", counter, elapsed);

    TEST_ASSERT_EQUAL(0, 0);
}

static void testInsertAllHashFunc(void) {
    StrArr_view sa_v = read("./test-data/probable-v2-wpa-top4800.txt");

    ht_HashFunction hash_functions[3] = {ht_HashDJB2, ht_HashSDBM, ht_HashLL};

    for (size_t n = 0; n < 3; n++) {    
        HT ht = ht_new(100*1000, hash_functions[n]);

        size_t **arr = calloc(sa_v.len, sizeof(int*));

        for (size_t i = 0; i < sa_v.len; i++) {
            size_t *v = malloc(sizeof(size_t));
            memcpy(v, &i, sizeof(size_t));
            arr[i] = v;
            int result = ht_insert(&ht, (unsigned char*)sa_v.arr[i], (void*)arr[i]);
            TEST_ASSERT_EQUAL(0, result);
        }

        ht_free(&ht);
        for (size_t i = 0; i < sa_v.len; i++) {
            free(arr[i]);
        }
        free(arr);
    }
}

static void testReadAllHashFunc(void) {
    StrArr_view sa_v = read("./test-data/probable-v2-wpa-top4800.txt");

    ht_HashFunction hash_functions[3] = {ht_HashDJB2, ht_HashSDBM, ht_HashLL};

    for (size_t n = 0; n < 3; n++) {    
        HT ht = ht_new(100*1000, hash_functions[n]);

        size_t **arr = calloc(sa_v.len, sizeof(int*));

        for (size_t i = 0; i < sa_v.len; i++) {
            size_t *v = malloc(sizeof(size_t));
            memcpy(v, &i, sizeof(size_t));
            arr[i] = v;
            int result = ht_insert(&ht, (unsigned char*)sa_v.arr[i], (void*)arr[i]);
            TEST_ASSERT_EQUAL(0, result);
        }
        
        for (size_t i = 0; i < sa_v.len; i++) {
            size_t *result = (size_t*)ht_read(&ht, (unsigned char*)sa_v.arr[i]);
            TEST_ASSERT_NOT_EQUAL(NULL, result);
            TEST_ASSERT_EQUAL(i, *result);
        }

        ht_free(&ht);
        for (size_t i = 0; i < sa_v.len; i++) {
            free(arr[i]);
        }
        free(arr);
    }
}

static void testDeleteAllHashfunc(void) {
    StrArr_view sa_v = read("./test-data/probable-v2-wpa-top4800.txt");

    ht_HashFunction hash_functions[3] = {ht_HashDJB2, ht_HashSDBM, ht_HashLL};

    for (size_t n = 0; n < 3; n++) {    
        HT ht = ht_new(100*1000, hash_functions[n]);

        size_t **arr = calloc(sa_v.len, sizeof(int*));

        for (size_t i = 0; i < sa_v.len; i++) {
            size_t *v = malloc(sizeof(size_t));
            memcpy(v, &i, sizeof(size_t));
            arr[i] = v;
            int result = ht_insert(&ht, (unsigned char*)sa_v.arr[i], (void*)arr[i]);
            TEST_ASSERT_EQUAL(0, result);
        }
        
        for (size_t i = 0; i < sa_v.len; i++) {
            size_t *result = (size_t*)ht_delete(&ht, (unsigned char*)sa_v.arr[i]);
            TEST_ASSERT_NOT_EQUAL(NULL, result);
            TEST_ASSERT_EQUAL(i, *result);
        }
        
        for (size_t i = 0; i < sa_v.len; i++) {
            size_t *result = (size_t*)ht_read(&ht, (unsigned char*)sa_v.arr[i]);
            TEST_ASSERT_EQUAL(NULL, result);
        }

        ht_free(&ht);
        for (size_t i = 0; i < sa_v.len; i++) {
            free(arr[i]);
        }
        free(arr);
    }
}

static void benchInsertAllHashFunc(void) {
    StrArr_view sa_v = read("./test-data/probable-v2-wpa-top4800.txt");

    ht_HashFunction hash_functions[3] = {ht_HashDJB2, ht_HashSDBM, ht_HashLL};
    char names[3][10] = {"dbj2", "sbdm", "loss loss"};

    for (size_t n = 0; n < 3; n++) {

        HT ht = ht_new(100*1000, hash_functions[n]);

        size_t **arr = calloc(sa_v.len, sizeof(int*));

        struct timeval begin, end;
        gettimeofday(&begin, 0);
        for (size_t i = 0; i < sa_v.len; i++) {
            size_t *v = malloc(sizeof(size_t));
            memcpy(v, &i, sizeof(size_t));
            arr[i] = v;
            int result = ht_insert(&ht, (unsigned char*)sa_v.arr[i], (void*)arr[i]);
            TEST_ASSERT_EQUAL(0, result);
        }
        
        gettimeofday(&end, 0);
        long seconds = end.tv_sec - begin.tv_sec;
        long microseconds = end.tv_usec - begin.tv_usec;
        double elapsed = seconds + microseconds*1e-6;
        printf("Inserting %lu entities with hash function <%s> took [ %f_sec ]\n", sa_v.len, names[n], elapsed);

        ht_free(&ht);
        for (size_t i = 0; i < sa_v.len; i++) {
            free(arr[i]);
        }
        free(arr);
    }
}

static void benchReadAllHashFunc(void) {
    StrArr_view sa_v = read("./test-data/probable-v2-wpa-top4800.txt");

    ht_HashFunction hash_functions[3] = {ht_HashDJB2, ht_HashSDBM, ht_HashLL};
    char names[3][10] = {"dbj2", "sbdm", "loss loss"};

    for (size_t n = 0; n < 3; n++) {
        HT ht = ht_new(100*1000, hash_functions[n]);

        size_t **arr = calloc(sa_v.len, sizeof(int*));

        for (size_t i = 0; i < sa_v.len; i++) {
            size_t *v = malloc(sizeof(size_t));
            memcpy(v, &i, sizeof(size_t));
            arr[i] = v;
            int result = ht_insert(&ht, (unsigned char*)sa_v.arr[i], (void*)arr[i]);
            TEST_ASSERT_EQUAL(0, result);
        }

        struct timeval begin, end;
        gettimeofday(&begin, 0);

        for (size_t i = 0; i < sa_v.len; i++) {
            size_t *result = (size_t*)ht_read(&ht, (unsigned char*)sa_v.arr[i]);
            TEST_ASSERT_NOT_EQUAL(NULL, result);
            TEST_ASSERT_EQUAL(i, *result);
        }
        
        gettimeofday(&end, 0);
        long seconds = end.tv_sec - begin.tv_sec;
        long microseconds = end.tv_usec - begin.tv_usec;
        double elapsed = seconds + microseconds*1e-6;
        printf("Reading %lu entities with hash function <%s> took [ %f_sec ]\n", sa_v.len, names[n], elapsed);

        ht_free(&ht);
        for (size_t i = 0; i < sa_v.len; i++) {
            free(arr[i]);
        }
        free(arr); 
    }
}

static void benchDeleteAllHashFunc(void) {
    StrArr_view sa_v = read("./test-data/probable-v2-wpa-top4800.txt");

    ht_HashFunction hash_functions[3] = {ht_HashDJB2, ht_HashSDBM, ht_HashLL};
    char names[3][10] = {"dbj2", "sbdm", "loss loss"};

    for (size_t n = 0; n < 3; n++) {
        HT ht = ht_new(100*1000, hash_functions[n]);

        size_t **arr = calloc(sa_v.len, sizeof(int*));

        for (size_t i = 0; i < sa_v.len; i++) {
            size_t *v = malloc(sizeof(size_t));
            memcpy(v, &i, sizeof(size_t));
            arr[i] = v;
            int result = ht_insert(&ht, (unsigned char*)sa_v.arr[i], (void*)arr[i]);
            TEST_ASSERT_EQUAL(0, result);
        }

        struct timeval begin, end;
        gettimeofday(&begin, 0);

        for (size_t i = 0; i < sa_v.len; i++) {
            size_t *result = (size_t*)ht_delete(&ht, (unsigned char*)sa_v.arr[i]);
            TEST_ASSERT_NOT_EQUAL(NULL, result);
            TEST_ASSERT_EQUAL(i, *result);
        }
        
        gettimeofday(&end, 0);
        long seconds = end.tv_sec - begin.tv_sec;
        long microseconds = end.tv_usec - begin.tv_usec;
        double elapsed = seconds + microseconds*1e-6;
        printf("Deleting %lu entities with hash function <%s> took [ %f_sec ]\n", sa_v.len, names[n], elapsed);

        ht_free(&ht);
        for (size_t i = 0; i < sa_v.len; i++) {
            free(arr[i]);
        }
        free(arr); 
    }
}

static void testIteratorAllhashfunc(void) {
    StrArr_view sa_v = read("./test-data/probable-v2-wpa-top4800.txt");

    ht_HashFunction hash_functions[3] = {ht_HashDJB2, ht_HashSDBM, ht_HashLL};

    for (size_t n = 0; n < 3; n++) {

        HT ht = ht_new(100*1000, hash_functions[n]);

        size_t **arr = calloc(sa_v.len, sizeof(int*));

        for (size_t i = 0; i < sa_v.len; i++) {
            size_t *v = malloc(sizeof(size_t));
            memcpy(v, &i, sizeof(size_t));
            arr[i] = v;
            int result = ht_insert(&ht, (unsigned char*)sa_v.arr[i], (void*)arr[i]);
            TEST_ASSERT_EQUAL(0, result);
        }

        size_t counter = 0;
        Iterator it = ht_newIterator();
        while (true) {
            Entity *en = ht_next(&ht, &it);
            if (!en) {
                break;
            }
            size_t *value = (size_t*)(en->value);
            TEST_ASSERT_NOT_EQUAL(NULL, value);
            counter++;
        }
        TEST_ASSERT_EQUAL(sa_v.len, counter);

        ht_free(&ht);
        for (size_t i = 0; i < sa_v.len; i++) {
            free(arr[i]);
        }
        free(arr);
    }
}

static void benchIteratorAllHashFunc(void) {
    StrArr_view sa_v = read("./test-data/probable-v2-wpa-top4800.txt");

    ht_HashFunction hash_functions[3] = {ht_HashDJB2, ht_HashSDBM, ht_HashLL};
    char names[3][10] = {"dbj2", "sbdm", "loss loss"};

    for (size_t n = 0; n < 3; n++) {
        HT ht = ht_new(100*1000, hash_functions[n]);

        size_t **arr = calloc(sa_v.len, sizeof(int*));

        for (size_t i = 0; i < sa_v.len; i++) {
            size_t *v = malloc(sizeof(size_t));
            memcpy(v, &i, sizeof(size_t));
            arr[i] = v;
            int result = ht_insert(&ht, (unsigned char*)sa_v.arr[i], (void*)arr[i]);
            TEST_ASSERT_EQUAL(0, result);
        }

        struct timeval begin, end;
        gettimeofday(&begin, 0);

        Iterator it = ht_newIterator();
        while (true) {
            Entity *en = ht_next(&ht, &it);
            if (!en) {
                break;
            }
            size_t *value = (size_t*)(en->value);
            TEST_ASSERT_NOT_EQUAL(NULL, value);
        }
        
        gettimeofday(&end, 0);
        long seconds = end.tv_sec - begin.tv_sec;
        long microseconds = end.tv_usec - begin.tv_usec;
        double elapsed = seconds + microseconds*1e-6;
        printf("Iterating over %lu entities with hash function <%s> took [ %f_sec ]\n", sa_v.len, names[n], elapsed);

        ht_free(&ht);
        for (size_t i = 0; i < sa_v.len; i++) {
            free(arr[i]);
        }
        free(arr); 
    }
}


int main(void)
{
    UnityBegin("ht_test.c");

    RUN_TEST(testHashCollisionsDjb2Success);
    RUN_TEST(testHashCollisionsSdbmSuccess);
    RUN_TEST(testHashCollisionsLL);
    
    RUN_TEST(bechHashCollisionsDjb2);
    RUN_TEST(benchHashCollisionsSdbm);
    RUN_TEST(benchHashCollisionsLL );

    RUN_TEST(testInsertAllHashFunc);
    RUN_TEST(testReadAllHashFunc);
    RUN_TEST(testDeleteAllHashfunc);
    RUN_TEST(testIteratorAllhashfunc);
    
    RUN_TEST(benchInsertAllHashFunc);
    RUN_TEST(benchReadAllHashFunc);
    RUN_TEST(benchDeleteAllHashFunc);
    RUN_TEST(benchIteratorAllHashFunc);


    
    return UnityEnd();
}
