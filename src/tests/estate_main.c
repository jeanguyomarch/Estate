#include "estate_suite.h"

START_TEST(_test_init)
{
   int count;

   count = estate_init();
   _ck_assert_int(count, ==, 1);

   count = estate_init();
   _ck_assert_int(count, ==, 2);

   count = estate_shutdown();
   _ck_assert_int(count, ==, 1);

   count = estate_shutdown();
   _ck_assert_int(count, ==, 0);
}
END_TEST

void
estate_test_init(TCase *tc)
{
   tcase_add_test(tc, _test_init);
}


