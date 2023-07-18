#pragma once
#include "Engine/allocations.cpp"
#include <stdint.h>

struct List {
    void* data;
    size_t size; // Total allocated space of the list in bytes.
    size_t used; // Length of the list in bytes.
    void Init(size_t size);
};

void List::Init(size_t size) {
    list->data = calloc(size, sizeof(uint8_t));
    list->used = 0;
    list->size = size;

void ListAppend(List* list, void* data, size_t size) {
}
