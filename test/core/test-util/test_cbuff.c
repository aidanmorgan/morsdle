//
// Created by aidan on 21/04/2022.
//

#include "unity.h"
#include "cbuff.h"

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

void test_cbuff_init() {
    cbuff_t* buff = &(cbuff_t){};
    uint8_t backing[16];

    cbuff_init(buff, (void**)&backing, 16, sizeof(uint8_t));

    TEST_ASSERT_EQUAL(16, buff->capacity);
}

void test_cbuff_insertempty() {
    cbuff_t* buff = &(cbuff_t){ };
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
    cbuff_t* buff = &(cbuff_t){ };
    uint8_t backing[16];

    cbuff_init(buff, (void**)&backing, 16, sizeof(uint8_t));

    TEST_ASSERT_FALSE(cbuff_canread(buff));
    uint8_t result = 0;
    TEST_ASSERT_FALSE(cbuff_read(buff, &result));
}

void test_cbuff_writefull() {
    cbuff_t* buff = &(cbuff_t){ };
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
    cbuff_t* buff = &(cbuff_t){ };
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
    cbuff_t* buff = &(cbuff_t){ };
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

void test_cbuff_complexobject() {
    typedef struct {
        uint32_t x;
        uint32_t y;
        double area;
    } rectangle_t;

    cbuff_t* buff = &(cbuff_t){ };
    rectangle_t backing[8];

    cbuff_init(buff, (void**)&backing, 4, sizeof(rectangle_t));

    TEST_ASSERT_TRUE(cbuff_write(buff, &(rectangle_t) { .x = 1, .y = 1, .area = 1.0}));
    TEST_ASSERT_TRUE(cbuff_write(buff, &(rectangle_t) { .x = 2, .y = 2, .area = 4.0}));
    TEST_ASSERT_TRUE(cbuff_write(buff, &(rectangle_t) { .x = 3, .y = 3, .area = 9.0}));
    TEST_ASSERT_TRUE(cbuff_write(buff, &(rectangle_t) { .x = 4, .y = 4, .area = 16.0}));

    rectangle_t read;
    TEST_ASSERT_TRUE(cbuff_read(buff, &read));
    TEST_ASSERT_EQUAL(1.0, read.area);

    TEST_ASSERT_TRUE(cbuff_read(buff, &read));
    TEST_ASSERT_EQUAL(4.0, read.area);

    TEST_ASSERT_TRUE(cbuff_read(buff, &read));
    TEST_ASSERT_EQUAL(9.0, read.area);

    TEST_ASSERT_TRUE(cbuff_read(buff, &read));
    TEST_ASSERT_EQUAL(16.0, read.area);
}

void test_cbuff_readmany() {
    cbuff_t* buff = &(cbuff_t){ };
    uint32_t backing[8];

    cbuff_init(buff, (void**)&backing, 8, sizeof(uint32_t));

    uint32_t value = 5;
    cbuff_write(buff, &value);
    value++;
    cbuff_write(buff, &value);
    value++;
    cbuff_write(buff, &value);
    value++;
    cbuff_write(buff, &value);

    uint32_t readback[4];

    size_t num_read = cbuff_readmany(buff, &readback, 4);
    TEST_ASSERT_EQUAL(4, num_read);

    TEST_ASSERT_EQUAL(5, readback[0]);
    TEST_ASSERT_EQUAL(6, readback[1]);
    TEST_ASSERT_EQUAL(7, readback[2]);
    TEST_ASSERT_EQUAL(8, readback[3]);
}

void test_cbuff_readmany_lessentries() {
    cbuff_t* buff = &(cbuff_t){ };
    uint32_t backing[8];

    cbuff_init(buff, (void**)&backing, 8, sizeof(uint32_t));

    uint32_t value = 10;
    cbuff_write(buff, &value);
    value*=2;
    cbuff_write(buff, &value);

    uint32_t readback[4];

    size_t num_read = cbuff_readmany(buff, &readback, 4);

    TEST_ASSERT_EQUAL(2, num_read);
    TEST_ASSERT_EQUAL(10, readback[0]);
    TEST_ASSERT_EQUAL(20, readback[1]);
}

void test_cbuff_peektail() {
    cbuff_t* buff = &(cbuff_t){ };
    uint32_t backing[8];

    cbuff_init(buff, (void**)&backing, 8, sizeof(uint32_t));

    uint32_t value = 1000;
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

    uint32_t readback;
    TEST_ASSERT_EQUAL(1, cbuff_peektail(buff, &readback, 1));
    TEST_ASSERT_EQUAL(1000, readback);
    TEST_ASSERT_EQUAL(0, buff->read_idx);

    uint32_t multireadback[3];
    TEST_ASSERT_EQUAL(3, cbuff_peektail(buff, &multireadback, 3));
    TEST_ASSERT_EQUAL(1000, multireadback[0]);
    TEST_ASSERT_EQUAL(1001, multireadback[1]);
    TEST_ASSERT_EQUAL(1002, multireadback[2]);
    TEST_ASSERT_EQUAL(0, buff->read_idx);

    // now we want to move the read index to the end of the buffer and to check the loop around, we will
    // need to move the write index as well as it's at the end of the array
    buff->read_idx = 6;
    buff->size = buff->size -1;

    // now we can add one more entry given we've shifted the pointer around
    value++;
    TEST_ASSERT_TRUE(cbuff_write(buff, &value));

    TEST_ASSERT_EQUAL(3, cbuff_peektail(buff, &multireadback, 3));
    TEST_ASSERT_EQUAL(1006, multireadback[0]);
    TEST_ASSERT_EQUAL(1007, multireadback[1]);
    TEST_ASSERT_EQUAL(1008, multireadback[2]);
}

void test_cbuff_peektailafter() {
    cbuff_t* buff = &(cbuff_t){ };
    uint32_t backing[32];

    cbuff_init(buff, (void**)&backing, 32, sizeof(uint8_t));

    for(uint8_t i = 0; i < 32; i++) {
        cbuff_write(buff, &i);
    }

    uint8_t value[5];
    cbuff_peektail_after(buff, &value, 0, 5);
    TEST_ASSERT_EQUAL(0, value[0]);
    TEST_ASSERT_EQUAL(1, value[1]);
    TEST_ASSERT_EQUAL(2, value[2]);
    TEST_ASSERT_EQUAL(3, value[3]);
    TEST_ASSERT_EQUAL(4, value[4]);

    cbuff_peektail_after(buff, &value, 27, 5);
    TEST_ASSERT_EQUAL(27, value[0]);
    TEST_ASSERT_EQUAL(28, value[1]);
    TEST_ASSERT_EQUAL(29, value[2]);
    TEST_ASSERT_EQUAL(30, value[3]);
    TEST_ASSERT_EQUAL(31, value[4]);
}

void test_cbuff_peekhead() {
    cbuff_t* buff = &(cbuff_t){ };
    uint32_t backing[8];

    cbuff_init(buff, (void**)&backing, 8, sizeof(uint32_t));

    uint32_t value = 1000;
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

    uint32_t readback;
    TEST_ASSERT_EQUAL(1, cbuff_peekhead(buff, &readback, 1));
    TEST_ASSERT_EQUAL(1007, readback);
    TEST_ASSERT_EQUAL(0, buff->write_idx);

    uint32_t multireadback[3];
    TEST_ASSERT_EQUAL(3, cbuff_peekhead(buff, &multireadback, 3));
    TEST_ASSERT_EQUAL(1005, multireadback[0]);
    TEST_ASSERT_EQUAL(1006, multireadback[1]);
    TEST_ASSERT_EQUAL(1007, multireadback[2]);
    TEST_ASSERT_EQUAL(0, buff->write_idx);

    // intentionally overrun the end of the buffer to check the boundary, to be able to do this
    // we need to move the read index first before trying to write another entry
    buff->read_idx = 2;
    buff->size = buff->size - 1;

    // now we can add one more entry given we've shifted the pointer around
    value++;
    TEST_ASSERT_TRUE(cbuff_write(buff, &value));


    TEST_ASSERT_EQUAL(3, cbuff_peekhead(buff, &multireadback, 3));
    TEST_ASSERT_EQUAL(1006, multireadback[0]);
    TEST_ASSERT_EQUAL(1007, multireadback[1]);
    TEST_ASSERT_EQUAL(1008, multireadback[2]);
    TEST_ASSERT_EQUAL(1, buff->write_idx);
}

void test_cbuff_seek() {
    cbuff_t* buff = &(cbuff_t){ };
    uint32_t backing[8];

    cbuff_init(buff, (void**)&backing, 8, sizeof(uint32_t));

    uint32_t value = 3000;
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

    TEST_ASSERT_TRUE(cbuff_seek(buff, 6));
    TEST_ASSERT_EQUAL(6, buff->read_idx);

    uint32_t readback;
    TEST_ASSERT_TRUE(cbuff_read(buff, &readback));
    TEST_ASSERT_EQUAL(3006, readback);
    TEST_ASSERT_TRUE(cbuff_read(buff, &readback));
    TEST_ASSERT_EQUAL(3007, readback);
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
    RUN_TEST(test_cbuff_complexobject);
    RUN_TEST(test_cbuff_readmany);
    RUN_TEST(test_cbuff_readmany_lessentries);
    RUN_TEST(test_cbuff_peekhead);
    RUN_TEST(test_cbuff_peektail);
    RUN_TEST(test_cbuff_peektailafter);
    RUN_TEST(test_cbuff_seek);

    return UNITY_END();
}
