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
}

void test_cbuff_insertempty() {
    cbuff_t buff = &(struct cbuff){ };
    uint8_t backing[16];

    cbuff_init(buff, (void**)&backing, 16, sizeof(uint8_t));

    TEST_ASSERT_TRUE(cbuff_canwrite(buff));
    uint8_t value = 32;
    TEST_ASSERT_TRUE(cbuff_write(buff, &value));

    TEST_ASSERT_TRUE(cbuff_canread( buff));

    uint8_t result = 0;
    TEST_ASSERT_TRUE(cbuff_read(buff, &result));

    TEST_ASSERT_EQUAL(32, result);
}

void test_cbuff_readempty() {
    cbuff_t buff = &(struct cbuff){ };
    uint8_t backing[16];

    cbuff_init(buff, (void**)&backing, 16, sizeof(uint8_t));

    TEST_ASSERT_FALSE(cbuff_canread(buff));
    uint8_t result = 0;
    TEST_ASSERT_FALSE(cbuff_read(buff, &result));
}

void test_cbuff_writefull() {
    cbuff_t buff = &(struct cbuff){ };
    uint32_t backing[8];

    cbuff_init(buff, (void**)&backing, 8, sizeof(uint32_t));

    uint32_t value = 1;
    TEST_ASSERT_TRUE(cbuff_write(buff, &value));
    value++;
    TEST_ASSERT_TRUE(cbuff_write(buff, &value));
    value++;
    TEST_ASSERT_TRUE(cbuff_write(buff, &value));
    value++;
    TEST_ASSERT_TRUE(cbuff_write(buff, &value));
    value++;
    TEST_ASSERT_TRUE(cbuff_write(buff, &value));
    value++;
    TEST_ASSERT_TRUE(cbuff_write(buff, &value));
    value++;
    TEST_ASSERT_TRUE(cbuff_write(buff, &value));
    value++;
    TEST_ASSERT_TRUE(cbuff_write(buff, &value));

    TEST_ASSERT_FALSE(cbuff_canwrite(buff));
    TEST_ASSERT_FALSE(cbuff_write(buff, &value));
}

void test_cbuff_fillandempty() {
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
}

void test_cbuff_alternatewriteandread() {
    cbuff_t buff = &(struct cbuff){ };
    uint32_t backing[8];

    cbuff_init(buff, (void**)&backing, 8, sizeof(uint32_t));

    // the maximum length of the backing buffer is 8 items, but we're going to keep chasing the
    // read and write pointers around the buffer, so we should be able to keep doing this over
    // and over
    for(uint32_t i = 0; i < 256; i++) {
        TEST_ASSERT_TRUE(cbuff_write(buff, &i));
        uint32_t read = 0;
        TEST_ASSERT_TRUE(cbuff_read(buff, &read));
        TEST_ASSERT_EQUAL(i, read);
    }

    TEST_ASSERT_EQUAL(buff->size, 0);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_cbuff_init);
    RUN_TEST(test_cbuff_insertempty);
    RUN_TEST(test_cbuff_writefull);
    RUN_TEST(test_cbuff_readempty);
    RUN_TEST(test_cbuff_fillandempty);
    RUN_TEST(test_cbuff_alternatewriteandread);
    return UNITY_END();
}
