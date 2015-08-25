#include "estate_suite.h"

#define MACH_STATES_COUNT       4
#define MACH_TRANSIT_COUNT      6

static Estate_Machine *_mach = NULL;

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

START_TEST(_test_mach_new)
{
   _mach = estate_machine_new(MACH_STATES_COUNT, MACH_TRANSIT_COUNT);
   _ck_assert_ptr(_mach, !=, NULL);
}
END_TEST

START_TEST(_test_mach_free)
{
   estate_machine_free(_mach);
   _mach = NULL;
}
END_TEST

void
estate_test_init(TCase *tc)
{
   tcase_add_test(tc, _test_init);
}

void
estate_test_machine(TCase *tc)
{
   tcase_add_test(tc, _test_mach_new);
   tcase_add_test(tc, _test_mach_free);
}

