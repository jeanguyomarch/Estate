#include "estate_private.h"

/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

EAPI Estate_State *
estate_state_new(Estate_Machine *mach EINA_UNUSED)
{
   /* XXX Use mach allocator */
   return calloc(1, sizeof(Estate_State));
}

EAPI void
estate_state_free(Estate_State *st)
{
   /* XXX Update when mach allocator will be used */
   free(st);
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
   EINA_SAFETY_ON_NULL_RETURN_VAL(transitions, EINA_FALSE);

   unsigned int i;

   st->transit = eina_array_new(transit_count);
   if (EINA_UNLIKELY(!st->transit))
     {
        CRI("Failed to create Transitions array");
        goto fail;
     }

   for (i = 0; i < transit_count; ++i)
     eina_array_push(st->transit, transitions[i]);

   /* Set enterer */
   st->cb[ESTATE_CB_TYPE_ENTERER].func = enterer;
   st->cb[ESTATE_CB_TYPE_ENTERER].data = NULL;
   if (enterer_datakey)
     st->cb[ESTATE_CB_TYPE_ENTERER].key = eina_stringshare_add(enterer_datakey);

   /* Set exiter */
   st->cb[ESTATE_CB_TYPE_EXITER].func = exiter;
   st->cb[ESTATE_CB_TYPE_EXITER].data = NULL;
   if (enterer_datakey)
     st->cb[ESTATE_CB_TYPE_EXITER].key = eina_stringshare_add(exiter_datakey);

   st->name = name;

   return EINA_TRUE;

fail:
   estate_state_deinit(st);
   return EINA_FALSE;
}

EAPI void
estate_state_deinit(Estate_State *st)
{
   if (!st) return;
   if (st->transit) eina_array_free(st->transit);
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

   if (wrp->func)
     {
        _estate_misc_cb_cache(mach, wrp);
        wrp->func(wrp->data, type, tr);
     }
}

