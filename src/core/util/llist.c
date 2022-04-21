//
// Created by aidan on 21/04/2022.
//

#include "include/llist.h"

static struct llist_node* _llist_new_node() {
    return (struct llist_node*)malloc(sizeof(struct llist_node));
}

llist_t llist_create() {
    llist_t list = (llist_t)malloc(sizeof(llist));
    list->count = 0;
    list->head = NULL;
    list->tail = NULL;

    return list;
}

void llist_dispose(llist_t list) {
    llist_dispose_all(list, NULL);
}
void llist_dispose_all(llist_t list, void (*deallocate)(void*)) {
    if(list->count == 0) {
        free(list);
    }
    else {
        struct llist_node* current = list->head;
        while(current != NULL) {
            struct llist_node* tmp = current->next;

            if(deallocate != NULL) {
                deallocate(current->data);
            }

            free(current);
            current = tmp;
        }
    }
}


void llist_insert(llist_t list, void* data) {
    struct llist_node* new_node = _llist_new_node();
    new_node->data = data;

    if(list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
    }
    else {
        struct llist_node *current = list->tail;
        current->next = new_node;
        new_node->previous = current;
        list->tail = new_node;
    }

    list->count++;
}

llist_find_result_t llist_find(llist_t list, bool (*filter)(void*,void*), void*params) {
    void* found[list->count];
    size_t found_count = 0;

    struct llist_node* current = list->head;
    while(current != NULL) {
        if(filter(params, current->data)) {
            found[found_count] = current->data;
            found_count++;
        }

        current = current->next;
    }

    llist_find_result* result = (llist_find_result*)malloc(sizeof(llist_find_result));
    result->count = found_count;

    if(found_count > 0) {
        result->items = malloc(sizeof(void *) * found_count);

        for (size_t i = 0; i < found_count; i++) {
            result->items[i] = found[i];
        }
    }
    else {
        result->items = NULL;
    }

    return (llist_find_result_t)result;
}

void llist_dispose_find_result(llist_find_result_t result) {
    free(result->items);
    free(result);
}


void llist_iterate(llist_t list, void (*it)(void*,void*), void*context) {
    struct llist_node* current = list->head;

    while(current != NULL) {
        it(context, current->data);
        current = current->next;
    }
}

void llist_remove_and_dispose(llist_t list, void* data, void (*deallocate)(void*)) {
    if(list->count == 0) {
        return;
    }

    if(data == list->head->data) {
        struct llist_node* node = list->head;

        if(list->head->next == NULL) {
            list->head = NULL;
            list->tail = NULL;
            list->count = 0;

            if(deallocate != NULL) {
                deallocate(node->data);
            }
            free(node);
            return;
        }
        else {
            list->head = node->next;
            list->count-=1;

            node->next->previous = NULL;

            if(deallocate != NULL) {
                deallocate(node->data);
            }
            free(node);
            return;
        }
    }

    if(data == list->tail->data) {
        struct llist_node* node = list->tail;

        if(node->previous == NULL) {
            list->head = NULL;
            list->tail = NULL;
            list->count = 0;

            if(deallocate != NULL) {
                deallocate(node->data);
            }
            free(node);

            return;
        }
        else {
            list->tail = node->previous;
            list->tail->next = NULL;
            list->count-=1;

            if(deallocate != NULL) {
                deallocate(node->data);
            }
            free(node);

            return;
        }
    }

    struct llist_node* current = list->head;
    while(current != NULL) {
        if(current->data == data) {
            struct llist_node* previous = current->previous;
            struct llist_node* next = current->next;

            previous->next = next;
            next->previous = previous;

            list->count-=1;

            if(deallocate != NULL) {
                deallocate(current->data);
            }
            free(current);

            return;
        }

        current = current->next;
    }

}

void llist_remove(llist_t list, void*data) {
    if(list->count == 0) {
        return;
    }

    llist_remove_and_dispose(list, data, NULL);
}