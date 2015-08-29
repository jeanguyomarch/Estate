#include "estate_private.h"

Eina_Bool
_estate_misc_cb_cache(Estate_Machine    *mach,
                      Estate_Cb_Wrapper *wrp)
{
   /* I want to minimize the access to the hash table that contains the
    * callbacks data. Upon the first call of the callback, the associated
    * data is cached and used for future use */
   if (wrp->key)
     {
        if (!wrp->data)
          {
             wrp->data = eina_hash_find(mach->data, wrp->key);
             if (EINA_UNLIKELY(!wrp->data))
               {
                  ERR("No data has been registered for \"%s\"", wrp->key);
                  return EINA_FALSE;
               }
          }
     }
   return EINA_TRUE;
}

void
_estate_misc_cb_call(Estate_Machine          *mach,
                     Estate_Cb_Wrapper       *wrp,
                     Estate_Cb_Type           type,
                     const Estate_Transition *tr)
{
   if (wrp->func)
     {
        _estate_misc_cb_cache(mach, wrp);
        mach->in_cb = type;
        wrp->result = wrp->func(wrp->data, type, tr);
        mach->in_cb = CB_UNLOCKED;
     }
}

