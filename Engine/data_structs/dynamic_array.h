#pragma once
#include <stdint.h>

typedef size_t index_t; // NOTE: Indexing is done with size_t for portability, but this makes it more clear what the size_t represents.


// NOTE: Generatinga list with DEFINE_LIST(T) is necessary before using. 
// EXAMPLE: DEFINE_LIST(int) will generate a list of ints.

#define DEFINE_LIST(T) \
\
/*This is the base struct for the list.*/ \
\
struct T##_List { \
    T* data; \
    index_t size; \
    index_t used; \
    index_t Init(index_t init_size = 4); \
    index_t Append(T data); \
}; \
\
/*list::Init(n) initializes the list to initial size of n (defaults to size 4)*/ \
\
index_t T##_List::Init(index_t init_size) { \
    this->data = (T*)calloc(init_size, sizeof(T)); \
    this->used = 0; \
    this->size = init_size; \
    return init_size; \
} \
\
/*list::Append(T value) appends the data to the end of the list in constant time (including resizing if necessary).*/ \
\
index_t T##_List::Append(T value) { \
    if (this->used >= this->size) { \
        this->size *=2; \
        this->data = (T*)realloc(this->data, this->size * sizeof(T)); \
    } \
    this->data[this->used++] = value; \
    return this->used; \
}\

