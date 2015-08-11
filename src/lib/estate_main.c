#include "estate_private.h"

static int _init_count = 0;

EAPI int
estate_init(void)
{
   if (++_init_count == 1)
     {
     }
   return _init_count;
}

EAPI int
estate_shutdown(void)
{
   if (--_init_count == 0)
     {
     }
   return _init_count;
}

