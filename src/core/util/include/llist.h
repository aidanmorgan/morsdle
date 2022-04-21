//
// Created by aidan on 21/04/2022.
//

#ifndef __LLIST_H__
#define __LLIST_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>

struct llist_node {
    void * data;
    struct llist_node* next;
    struct llist_node* previous;
};

typedef struct {
    struct llist_node* head;
    struct llist_node* tail;
    size_t count;
} llist;
typedef llist* llist_t;

struct llist_find_result {
    void** items;
    size_t count;
};
typedef struct llist_find_result* llist_find_result_t;

llist_t llist_create();
void llist_insert(llist_t list, void* data);
void llist_remove(llist_t list, void*data);
void llist_remove_and_free(llist_t list, void* data, void (*deallocate)(void*));

void llist_iterate(llist_t list, void (*it)(void*,void*), void*context);
llist_find_result_t llist_find(llist_t list, bool (*filter)(void*,void*), void*params);
void llist_find_result_free(llist_find_result_t result);

#endif //__LLIST_H__
