//
// Created by aidan on 21/04/2022.
//

#include "unity.h"
#include "cbuff.h"

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

void test_cbuff_init() {
    cbuff_t buff = &(struct cbuff){};
    uint8_t backing[16];

    cbuff_init(buff, (void**)&backing, 16, sizeof(uint8_t));

    TEST_ASSERT_EQUAL(16, buff->capacity);
    TEST_ASSERT_EQUAL(0, buff->write_idx);
    TEST_ASSERT_EQUAL(0, buff->read_idx);
}

void test_cbuff_insert_empty() {
    cbuff_t buff = &(struct cbuff){ };
    uint8_t backing[16];

    cbuff_init(buff, (void**)&backing, 16, sizeof(uint8_t));

    TEST_ASSERT_TRUE(cbuff_canwrite(buff));
    uint8_t value = 32;
    cbuff_write(buff, &value);

    TEST_ASSERT_TRUE(cbuff_canread( buff));

    uint8_t result = 0;
    cbuff_read(buff, &result);

    TEST_ASSERT_EQUAL(32, result);
}

void test_cbuff_read_empty() {
    cbuff_t buff = &(struct cbuff){ };
    uint8_t backing[16];

    cbuff_init(buff, (void**)&backing, 16, sizeof(uint8_t));

    TEST_ASSERT_FALSE(cbuff_canread(buff));
    uint8_t result = 0;
    TEST_ASSERT_FALSE(cbuff_read(buff, &result));
}

void test_cbuff_write_full() {
    cbuff_t buff = &(struct cbuff){ };
    uint32_t backing[8];

    cbuff_init(buff, (void**)&backing, 8, sizeof(uint32_t));

    uint32_t value = 1;
    cbuff_write(buff, &value);
    value++;
    cbuff_write(buff, &value);
    value++;
    cbuff_write(buff, &value);
    value++;
    cbuff_write(buff, &value);
    value++;
    cbuff_write(buff, &value);
    value++;
    cbuff_write(buff, &value);
    value++;
    cbuff_write(buff, &value);
    value++;
    cbuff_write(buff, &value);

    TEST_ASSERT_FALSE(cbuff_canwrite(buff));
    TEST_ASSERT_FALSE(cbuff_write(buff, &value));
}

void test_cbuff_fill_and_empty() {
    cbuff_t buff = &(struct cbuff){ };
    uint32_t backing[8];

    cbuff_init(buff, (void**)&backing, 8, sizeof(uint32_t));

    uint32_t value = 100;
    while(cbuff_canwrite(buff)) {
        cbuff_write(buff, &value);
        value += 100;
    }

    uint32_t count = 100;
    while(cbuff_canread(buff)) {
        uint32_t result = 0;
        cbuff_read(buff, &result);
        TEST_ASSERT_EQUAL(count, result);
        count += 100;
    }

    TEST_ASSERT_EQUAL(buff->read_idx, buff->write_idx);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_cbuff_init);
    RUN_TEST(test_cbuff_insert_empty);
    RUN_TEST(test_cbuff_write_full);
    RUN_TEST(test_cbuff_read_empty);
    RUN_TEST(test_cbuff_fill_and_empty);
    return UNITY_END();
}
