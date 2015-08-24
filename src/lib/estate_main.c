#include "estate_private.h"

static int _init_count = 0;
int _estate_log_dom = -1;

#if 0
int ESTATE_EVENT_ENTERER = -1;
int ESTATE_EVENT_EXITER = -1;
int ESTATE_EVENT_TRANSITION = -1;
#endif

EAPI int
estate_init(void)
{
   if (++_init_count == 1)
     {
        if (EINA_UNLIKELY(!eina_init()))
          {
             EINA_LOG_CRIT("Failed to init Eina");
             return --_init_count;
          }
        _estate_log_dom = eina_log_domain_register("Estate", EINA_COLOR_WHITE);
        if (EINA_UNLIKELY(_estate_log_dom < 0))
          {
             EINA_LOG_CRIT("Failed to create log domain");
             eina_shutdown();
             return --_init_count;
          }
#if 0
        ESTATE_EVENT_ENTERER = ecore_event_type_new();
        ESTATE_EVENT_EXITER = ecore_event_type_new();
        ESTATE_EVENT_TRANSITION = ecore_event_type_new();
#endif
     }
   return _init_count;
}

EAPI int
estate_shutdown(void)
{
   if (--_init_count == 0)
     {
        eina_log_domain_unregister(_estate_log_dom);
        eina_shutdown();
     }
   return _init_count;
}

