#pragma once
#include "Engine/allocations.cpp"

struct List {
    void* data;
    size_t size; // Total allocated space of the list.
    size_t used; // Length of the list in bytes.
};

void ListInit(List* list, size_t size) {
    list->data = skymem::alloc(size);
    list->used = 0;
    list->size = size;

void ListAppend(List* list, void* data, size_t size) {
}
