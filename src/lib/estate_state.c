#include "estate_private.h"

/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

EAPI Estate_State *
estate_state_new(Estate_Machine *mach,
                 unsigned int    transit_count)
{
   Estate_State *st = NULL;

   st = _estate_mempool_state_push(mach->mempool);
   if (EINA_UNLIKELY(!st))
     goto fail;

   if (transit_count != 0)
     {
        st->transit = calloc(transit_count, sizeof(Estate_Transition *));
        if (EINA_UNLIKELY(!st->transit))
          {
             CRI("Failed to allocate array");
             goto fail;
          }
     }
   st->transit_count = transit_count;

   return st;

fail:
   free(st);
   return NULL;
}

EAPI void
estate_state_free(Estate_State *st)
{
   estate_state_deinit(st);
   free(st->transit);
   /* State will be freed by the machine */
}

EAPI Eina_Bool
estate_state_init(Estate_State             *st,
                  const char               *name,
                  const Estate_Transition **transitions,
                  unsigned int              transit_count,
                  Estate_Cb                 enterer,
                  const char               *enterer_datakey,
                  Estate_Cb                 exiter,
                  const char               *exiter_datakey)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(st, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(transit_count != st->transit_count, EINA_FALSE);

   /* Copy the transition pointers if necessary */
   if (transitions)
     memcpy(st->transit, transitions,
            transit_count * sizeof(Estate_Transition *));

   /* Set enterer */
   st->cb[0].func = enterer;
   st->cb[0].data = NULL;
   st->cb[0].result = ESTATE_CB_OK;
   st->cb[0].key = (enterer_datakey != NULL) ?
      eina_stringshare_add(enterer_datakey) : NULL;

   /* Set exiter */
   st->cb[1].func = exiter;
   st->cb[1].data = NULL;
   st->cb[1].result = ESTATE_CB_OK;
   st->cb[1].key = (exiter_datakey != NULL) ?
      eina_stringshare_add(exiter_datakey) : NULL;

   st->name = eina_stringshare_add(name);

   return EINA_TRUE;
}

EAPI void
estate_state_deinit(Estate_State *st)
{
   if (!st) return;
   if (st->name) eina_stringshare_del(st->name);
   if (st->cb[0].key) eina_stringshare_del(st->cb[0].key);
   if (st->cb[1].key) eina_stringshare_del(st->cb[1].key);
}

EAPI const char *
estate_state_name_get(const Estate_State *st)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(st, NULL);
   return st->name;
}

void
_estate_state_cb_call(Estate_Machine          *mach,
                      Estate_State            *st,
                      const Estate_Transition *tr,
                      Estate_Cb_Type           type)
{
   Estate_Cb_Wrapper *wrp = &(st->cb[type]);
   _estate_misc_cb_call(mach, wrp, type, tr);
}

EAPI int
estate_state_sort_transitions_cb(const void *d1,
                                 const void *d2)
{
#define CAST(var) (const Estate_Transition *)(*(Estate_Transition * const *)var)
   const Estate_Transition *t1 = CAST(d1);
   const Estate_Transition *t2 = CAST(d2);

   /* Sort by stringshares */
   return (int)(t1->name - t2->name);
}

