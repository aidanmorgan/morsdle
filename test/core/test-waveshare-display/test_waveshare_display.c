#include "unity.h"
#include "waveshare_image_buffer.h"

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {}

void do_get_test(uint8_t val, uint8_t one, uint8_t two, uint8_t three, uint8_t four) {
    TEST_ASSERT_EQUAL(one, GET_VAL(val, 2, 0));
    TEST_ASSERT_EQUAL(two, GET_VAL(val, 2, 2));
    TEST_ASSERT_EQUAL(three, GET_VAL(val, 2, 4));
    TEST_ASSERT_EQUAL(four, GET_VAL(val, 2, 6));
}

void waveshare_ib_test_bit_set_macro() {
    // 00 00 00 00
    do_get_test(0, 0, 0, 0, 0);
    // 00 00 00 01
    do_get_test(1, 1, 0, 0, 0);
    // 00 00 00 10
    do_get_test(2, 2, 0, 0, 0);
    // 00 00 00 11
    do_get_test(3, 3, 0, 0, 0);
    // 00 00 01 00
    do_get_test(4, 0, 1, 0, 0);
    // 00 00 01 01
    do_get_test(5, 1, 1, 0, 0);
    // 00 00 01 10
    do_get_test(6, 2, 1, 0, 0);
    // 00 00 01 11
    do_get_test(7, 3, 1, 0, 0);
    // 00 00 10 00
    do_get_test(8, 0, 2, 0, 0);
    // 00 00 10 01
    do_get_test(9, 1, 2, 0, 0);
    // 00 00 10 10
    do_get_test(10, 2, 2, 0, 0);
    // 00 00 10 11
    do_get_test(11, 3, 2, 0, 0);
    // 00 00 11 00
    do_get_test(12, 0, 3, 0, 0);
    // 00 00 11 01
    do_get_test(13, 1, 3, 0, 0);
    // 00 00 11 10
    do_get_test(14, 2, 3, 0, 0);
    // 00 00 11 11
    do_get_test(15, 3, 3, 0, 0);
    // 00 01 00 00
    do_get_test(16, 0, 0, 1, 0);
    // 00 01 00 01
    do_get_test(17, 1, 0, 1, 0);
    // 00 01 00 10
    do_get_test(18, 2, 0, 1, 0);
    // 00 01 00 11
    do_get_test(19, 3, 0, 1, 0);
    // 00 01 01 00
    do_get_test(20, 0, 1, 1, 0);
    // 00 01 01 01
    do_get_test(21, 1, 1, 1, 0);
    // 00 01 01 10
    do_get_test(22, 2, 1, 1, 0);
    // 00 01 01 11
    do_get_test(23, 3, 1, 1, 0);
    // 00 01 10 00
    do_get_test(24, 0, 2, 1, 0);
    // 00 01 10 01
    do_get_test(25, 1, 2, 1, 0);
    // 00 01 10 10
    do_get_test(26, 2, 2, 1, 0);
    // 00 01 10 11
    do_get_test(27, 3, 2, 1, 0);
    // 00 01 11 00
    do_get_test(28, 0, 3, 1, 0);
    // 00 01 11 01
    do_get_test(29, 1, 3, 1, 0);
    // 00 01 11 10
    do_get_test(30, 2, 3, 1, 0);
    // 00 01 11 11
    do_get_test(31, 3, 3, 1, 0);
    // 00 10 00 00
    do_get_test(32, 0, 0, 2, 0);
    // 00 10 00 01
    do_get_test(33, 1, 0, 2, 0);
    // 00 10 00 10
    do_get_test(34, 2, 0, 2, 0);
    // 00 10 00 11
    do_get_test(35, 3, 0, 2, 0);
    // 00 10 01 00
    do_get_test(36, 0, 1, 2, 0);
    // 00 10 01 01
    do_get_test(37, 1, 1, 2, 0);
    // 00 10 01 10
    do_get_test(38, 2, 1, 2, 0);
    // 00 10 01 11
    do_get_test(39, 3, 1, 2, 0);
    // 00 10 10 00
    do_get_test(40, 0, 2, 2, 0);
    // 00 10 10 01
    do_get_test(41, 1, 2, 2, 0);
    // 00 10 10 10
    do_get_test(42, 2, 2, 2, 0);
    // 00 10 10 11
    do_get_test(43, 3, 2, 2, 0);
    // 00 10 11 00
    do_get_test(44, 0, 3, 2, 0);
    // 00 10 11 01
    do_get_test(45, 1, 3, 2, 0);
    // 00 10 11 10
    do_get_test(46, 2, 3, 2, 0);
    // 00 10 11 11
    do_get_test(47, 3, 3, 2, 0);
    // 00 11 00 00
    do_get_test(48, 0, 0, 3, 0);
    // 00 11 00 01
    do_get_test(49, 1, 0, 3, 0);
    // 00 11 00 10
    do_get_test(50, 2, 0, 3, 0);
    // 00 11 00 11
    do_get_test(51, 3, 0, 3, 0);
    // 00 11 01 00
    do_get_test(52, 0, 1, 3, 0);
    // 00 11 01 01
    do_get_test(53, 1, 1, 3, 0);
    // 00 11 01 10
    do_get_test(54, 2, 1, 3, 0);
    // 00 11 01 11
    do_get_test(55, 3, 1, 3, 0);
    // 00 11 10 00
    do_get_test(56, 0, 2, 3, 0);
    // 00 11 10 01
    do_get_test(57, 1, 2, 3, 0);
    // 00 11 10 10
    do_get_test(58, 2, 2, 3, 0);
    // 00 11 10 11
    do_get_test(59, 3, 2, 3, 0);
    // 00 11 11 00
    do_get_test(60, 0, 3, 3, 0);
    // 00 11 11 01
    do_get_test(61, 1, 3, 3, 0);
    // 00 11 11 10
    do_get_test(62, 2, 3, 3, 0);
    // 00 11 11 11
    do_get_test(63, 3, 3, 3, 0);
    // 01 00 00 00
    do_get_test(64, 0, 0, 0, 1);
    // 01 00 00 01
    do_get_test(65, 1, 0, 0, 1);
    // 01 00 00 10
    do_get_test(66, 2, 0, 0, 1);
    // 01 00 00 11
    do_get_test(67, 3, 0, 0, 1);
    // 01 00 01 00
    do_get_test(68, 0, 1, 0, 1);
    // 01 00 01 01
    do_get_test(69, 1, 1, 0, 1);
    // 01 00 01 10
    do_get_test(70, 2, 1, 0, 1);
    // 01 00 01 11
    do_get_test(71, 3, 1, 0, 1);
    // 01 00 10 00
    do_get_test(72, 0, 2, 0, 1);
    // 01 00 10 01
    do_get_test(73, 1, 2, 0, 1);
    // 01 00 10 10
    do_get_test(74, 2, 2, 0, 1);
    // 01 00 10 11
    do_get_test(75, 3, 2, 0, 1);
    // 01 00 11 00
    do_get_test(76, 0, 3, 0, 1);
    // 01 00 11 01
    do_get_test(77, 1, 3, 0, 1);
    // 01 00 11 10
    do_get_test(78, 2, 3, 0, 1);
    // 01 00 11 11
    do_get_test(79, 3, 3, 0, 1);
    // 01 01 00 00
    do_get_test(80, 0, 0, 1, 1);
    // 01 01 00 01
    do_get_test(81, 1, 0, 1, 1);
    // 01 01 00 10
    do_get_test(82, 2, 0, 1, 1);
    // 01 01 00 11
    do_get_test(83, 3, 0, 1, 1);
    // 01 01 01 00
    do_get_test(84, 0, 1, 1, 1);
    // 01 01 01 01
    do_get_test(85, 1, 1, 1, 1);
    // 01 01 01 10
    do_get_test(86, 2, 1, 1, 1);
    // 01 01 01 11
    do_get_test(87, 3, 1, 1, 1);
    // 01 01 10 00
    do_get_test(88, 0, 2, 1, 1);
    // 01 01 10 01
    do_get_test(89, 1, 2, 1, 1);
    // 01 01 10 10
    do_get_test(90, 2, 2, 1, 1);
    // 01 01 10 11
    do_get_test(91, 3, 2, 1, 1);
    // 01 01 11 00
    do_get_test(92, 0, 3, 1, 1);
    // 01 01 11 01
    do_get_test(93, 1, 3, 1, 1);
    // 01 01 11 10
    do_get_test(94, 2, 3, 1, 1);
    // 01 01 11 11
    do_get_test(95, 3, 3, 1, 1);
    // 01 10 00 00
    do_get_test(96, 0, 0, 2, 1);
    // 01 10 00 01
    do_get_test(97, 1, 0, 2, 1);
    // 01 10 00 10
    do_get_test(98, 2, 0, 2, 1);
    // 01 10 00 11
    do_get_test(99, 3, 0, 2, 1);
    // 01 10 01 00
    do_get_test(100, 0, 1, 2, 1);
    // 01 10 01 01
    do_get_test(101, 1, 1, 2, 1);
    // 01 10 01 10
    do_get_test(102, 2, 1, 2, 1);
    // 01 10 01 11
    do_get_test(103, 3, 1, 2, 1);
    // 01 10 10 00
    do_get_test(104, 0, 2, 2, 1);
    // 01 10 10 01
    do_get_test(105, 1, 2, 2, 1);
    // 01 10 10 10
    do_get_test(106, 2, 2, 2, 1);
    // 01 10 10 11
    do_get_test(107, 3, 2, 2, 1);
    // 01 10 11 00
    do_get_test(108, 0, 3, 2, 1);
    // 01 10 11 01
    do_get_test(109, 1, 3, 2, 1);
    // 01 10 11 10
    do_get_test(110, 2, 3, 2, 1);
    // 01 10 11 11
    do_get_test(111, 3, 3, 2, 1);
    // 01 11 00 00
    do_get_test(112, 0, 0, 3, 1);
    // 01 11 00 01
    do_get_test(113, 1, 0, 3, 1);
    // 01 11 00 10
    do_get_test(114, 2, 0, 3, 1);
    // 01 11 00 11
    do_get_test(115, 3, 0, 3, 1);
    // 01 11 01 00
    do_get_test(116, 0, 1, 3, 1);
    // 01 11 01 01
    do_get_test(117, 1, 1, 3, 1);
    // 01 11 01 10
    do_get_test(118, 2, 1, 3, 1);
    // 01 11 01 11
    do_get_test(119, 3, 1, 3, 1);
    // 01 11 10 00
    do_get_test(120, 0, 2, 3, 1);
    // 01 11 10 01
    do_get_test(121, 1, 2, 3, 1);
    // 01 11 10 10
    do_get_test(122, 2, 2, 3, 1);
    // 01 11 10 11
    do_get_test(123, 3, 2, 3, 1);
    // 01 11 11 00
    do_get_test(124, 0, 3, 3, 1);
    // 01 11 11 01
    do_get_test(125, 1, 3, 3, 1);
    // 01 11 11 10
    do_get_test(126, 2, 3, 3, 1);
    // 10 11 11 11
    do_get_test(127, 3, 3, 3, 1);
    // 11 00 00 00
    do_get_test(128, 0, 0, 0, 2);
    // 11 00 00 01
    do_get_test(129, 1, 0, 0, 2);
    // 11 00 00 10
    do_get_test(130, 2, 0, 0, 2);
    // 11 00 00 11
    do_get_test(131, 3, 0, 0, 2);



}

void do_set_test(uint8_t expected, uint8_t one, uint8_t two, uint8_t three, uint8_t four) {
    uint8_t data = 0;
    data = SET_VAL(data, 2, 0, one);
    data = SET_VAL(data, 2, 2, two);
    data = SET_VAL(data, 2, 4, three);
    data = SET_VAL(data, 2, 6, four);

    TEST_ASSERT_EQUAL(expected, data);
}

void waveshare_ib_test_bit_get_macro() {
    do_set_test(0, 0, 0 ,0 ,0);
    do_set_test(1, 1, 0 ,0 ,0);
    do_set_test(4, 0, 1 ,0 ,0);
    do_set_test(16, 0, 0 ,1 ,0);
    do_set_test(64, 0, 0 ,0 ,1);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(waveshare_ib_test_bit_set_macro);
    RUN_TEST(waveshare_ib_test_bit_get_macro);

    return UNITY_END();
}
