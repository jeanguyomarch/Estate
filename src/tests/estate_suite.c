#include "estate_suite.h"

typedef struct
{
   const char *test_case;
   void (*build)(TCase *tc);
} Estate_Test_Case;

static const Estate_Test_Case _etc[] =
{
   { "Init", estate_test_init },
   { NULL, NULL }
};

int
main(void)
{
   Suite *s;
   SRunner *sr;
   TCase *tc;
   int i, failed_count;

   s = suite_create("Estate");
   for (i = 0; _etc[i].test_case; ++i)
     {
        tc = tcase_create(_etc[i].test_case);
        tcase_set_timeout(tc, 0);
        _etc[i].build(tc);
        suite_add_tcase(s, tc);
     }

   sr = srunner_create(s);
   srunner_set_xml(sr, "estate_suite.xml");

   srunner_run_all(sr, CK_ENV);
   failed_count = srunner_ntests_failed(sr);
   srunner_free(sr);

   return (failed_count == 0) ? 0 : -1;
}

