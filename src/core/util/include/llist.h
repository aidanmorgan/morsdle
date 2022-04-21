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

typedef struct  {
    void** items;
    size_t count;
} llist_find_result;
typedef llist_find_result* llist_find_result_t;

llist_t llist_create();
void llist_dispose(llist_t list);
void llist_dispose_all(llist_t list, void (*deallocate)(void*));

void llist_insert(llist_t list, void* data);

void llist_remove(llist_t list, void*data);
void llist_remove_and_dispose(llist_t list, void* data, void (*deallocate)(void*));

void llist_iterate(llist_t list, void (*it)(void*,void*), void*context);

llist_find_result_t llist_find(llist_t list, bool (*filter)(void*,void*), void*params);
void llist_dispose_find_result(llist_find_result_t result);

#endif //__LLIST_H__
