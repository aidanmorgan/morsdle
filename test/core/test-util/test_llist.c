//
// Created by aidan on 21/04/2022.
//

#include "unity.h"
#include "llist.h"

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

void test_llist_create() {
    llist_t list = llist_create();
    TEST_ASSERT_NOT_NULL(list);
    TEST_ASSERT_NULL(list->head);
    TEST_ASSERT_NULL(list->tail);
    TEST_ASSERT_EQUAL(0, list->count);
    llist_dispose(list);
}

void test_llist_insert() {
    llist_t list = llist_create();
    llist_insert(list, (void*)5);
    TEST_ASSERT_EQUAL(1, list->count);
    TEST_ASSERT_NOT_NULL(list->head);
    TEST_ASSERT_NOT_NULL(list->tail);
    TEST_ASSERT_EQUAL(list->head, list->tail);
    TEST_ASSERT_NULL(list->head->previous);
    TEST_ASSERT_NULL(list->head->next);
    TEST_ASSERT_NULL(list->tail->next);
    TEST_ASSERT_NULL(list->tail->previous);
    TEST_ASSERT_EQUAL((void*)5, list->head->data);

    llist_insert(list, (void*)6);
    TEST_ASSERT_EQUAL(2, list->count);
    TEST_ASSERT_NOT_EQUAL(list->head, list->tail);
    TEST_ASSERT_EQUAL(list->head->next, list->tail);
    TEST_ASSERT_EQUAL(list->tail->previous, list->head);
    TEST_ASSERT_EQUAL((void*)5, list->head->data);
    TEST_ASSERT_EQUAL((void*)6, list->tail->data);

    llist_insert(list, (void*)7);
    TEST_ASSERT_EQUAL(3, list->count);
    TEST_ASSERT_EQUAL((void*)5, list->head->data);
    TEST_ASSERT_EQUAL((void*)6, list->tail->previous->data);
    TEST_ASSERT_EQUAL((void*)6, list->head->next->data);
    TEST_ASSERT_EQUAL((void*)7, list->tail->data);

    llist_dispose(list);
}

void test_llist_remove() {
    llist_t list = llist_create();
    llist_insert(list, (void*)5);

    // test removing the last entry in the list
    llist_remove(list, (void*)5);
    TEST_ASSERT_EQUAL(0, list->count);
    TEST_ASSERT_NULL(list->head);
    TEST_ASSERT_NULL(list->tail);
    llist_dispose(list);

    // test removing the first entry in the list
    list = llist_create();
    llist_insert(list, (void*)5);
    llist_insert(list, (void*)6);
    llist_remove(list, (void*)5);
    TEST_ASSERT_EQUAL(1, list->count);
    TEST_ASSERT_EQUAL((void*)6, list->head->data);
    TEST_ASSERT_EQUAL((void*)6, list->tail->data);
    llist_dispose(list);

    // test removing the last entry in the list
    list = llist_create();
    llist_insert(list, (void*)5);
    llist_insert(list, (void*)6);
    llist_remove(list, (void*)6);
    TEST_ASSERT_EQUAL(1, list->count);
    TEST_ASSERT_EQUAL((void*)5, list->head->data);
    TEST_ASSERT_EQUAL((void*)5, list->tail->data);
    llist_dispose(list);

    // test removing a middle entry
    list = llist_create();
    llist_insert(list, (void*)5);
    llist_insert(list, (void*)6);
    llist_insert(list, (void*)7);
    llist_remove(list, (void*)6);
    TEST_ASSERT_EQUAL(2, list->count);
    TEST_ASSERT_EQUAL((void*)5, list->head->data);
    TEST_ASSERT_EQUAL((void*)7, list->tail->data);
    TEST_ASSERT_EQUAL(list->head->next, list->tail);
    TEST_ASSERT_EQUAL(list->tail->previous, list->head);
    llist_dispose(list);
}

static bool _intfinder(void*ctx, void*d) {
    uint8_t* context = (uint8_t*)ctx;
    uint8_t* data = (uint8_t*)d;

    return context == data;
}

void test_llist_find() {
    llist_t list = llist_create();

    // try and find in an empty list
    llist_find_result_t  result = llist_find(list, _intfinder, (void*)5);
    TEST_ASSERT_EQUAL(0, result->count);
    TEST_ASSERT_NULL(result->items);
    llist_dispose_find_result(result);

    // find when it's the only entry in the list
    llist_insert(list, (void*)5);
    result = llist_find(list, _intfinder, (void*)5);
    TEST_ASSERT_EQUAL(1, result->count);
    llist_dispose_find_result(result);

    // find a value that is not in the list
    result = llist_find(list, _intfinder, (void*)6);
    TEST_ASSERT_EQUAL(0, result->count);
    llist_dispose_find_result(result);

    // find multiple values in the list
    llist_insert(list, (void*)5);
    result = llist_find(list, _intfinder, (void*)5);
    TEST_ASSERT_EQUAL(2, result->count);
    llist_dispose_find_result(result);

    llist_dispose(list);
}

static void _iterate(void* ctx, void* d) {
    uint8_t* context = (uint8_t*)ctx;

    // this is test code, yes this is dodgy, don't care.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wvoid-pointer-to-int-cast"
    uint8_t data = (uint8_t)d;
#pragma clang diagnostic pop

    *(context) += data;
}

void test_llist_iterate() {
    llist_t list = llist_create();

    // try and iterate an empty list
    uint8_t total = 0;
    llist_iterate(list, _iterate, (void*)&total);
    TEST_ASSERT_EQUAL(0, total);

    // iterate one entry - sum should be 5
    total = 0;
    llist_insert(list, (void*)5);
    llist_iterate(list, _iterate, (void*)&total);
    TEST_ASSERT_EQUAL(5, total);

    // iterate two entries - sum should be 15
    total = 0;
    llist_insert(list, (void*)10);
    llist_iterate(list, _iterate, (void*)&total);
    TEST_ASSERT_EQUAL(15, total);

    // iterate three entries - sum should be 30
    total = 0;
    llist_insert(list, (void*)15);
    llist_iterate(list, _iterate, (void*)&total);
    TEST_ASSERT_EQUAL(30, total);

    llist_dispose(list);
}

void test_llist_clear() {
    llist_t list = llist_create();

    // test clearing an empty list
    llist_clear(list);
    TEST_ASSERT_EQUAL(0, list->count);
    TEST_ASSERT_NULL(list->head);
    TEST_ASSERT_NULL(list->tail);

    // test clearing one item
    llist_insert(list, (void*)5);
    llist_clear(list);
    TEST_ASSERT_EQUAL(0, list->count);
    TEST_ASSERT_NULL(list->head);
    TEST_ASSERT_NULL(list->tail);

    // test clearing multiple items
    llist_insert(list, (void*)1);
    llist_insert(list, (void*)2);
    llist_insert(list, (void*)3);
    llist_insert(list, (void*)4);
    llist_insert(list, (void*)5);
    llist_clear(list);
    TEST_ASSERT_EQUAL(0, list->count);
    TEST_ASSERT_NULL(list->head);
    TEST_ASSERT_NULL(list->tail);



    llist_dispose(list);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_llist_create);
    RUN_TEST(test_llist_insert);
    RUN_TEST(test_llist_remove);
    RUN_TEST(test_llist_find);
    RUN_TEST(test_llist_iterate);
    RUN_TEST(test_llist_clear);

    return UNITY_END();
}
