#include "estate_private.h"

static int _init_count = 0;
int _estate_log_dom = -1;

EAPI int
estate_init(void)
{
   if (++_init_count == 1)
     {
        _estate_log_dom = eina_log_domain_register("Estate", EINA_COLOR_WHITE);
        if (EINA_UNLIKELY(_estate_log_dom < 0))
          {
             EINA_LOG_CRIT("Failed to create log domain");
             return --_init_count;
          }
     }
   return _init_count;
}

EAPI int
estate_shutdown(void)
{
   if (--_init_count == 0)
     {
        eina_log_domain_unregister(_estate_log_dom);
     }
   return _init_count;
}

