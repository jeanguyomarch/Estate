#include "estate_cc.h"

Eina_Bool
estate_cc_log_init(void)
{
   _estate_cc_dom = eina_log_domain_register("estate_cc", EINA_COLOR_RED);
   if (_estate_cc_dom < 0)
     {
        EINA_LOG_CRIT("Failed to init log domain");
        return EINA_FALSE;
     }
   return EINA_TRUE;
}

void
estate_cc_log_shutdown(void)
{
   eina_log_domain_unregister(_estate_cc_dom);
}

