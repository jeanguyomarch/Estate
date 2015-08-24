#include "estate_private.h"

EAPI Estate_Transition *
estate_transition_new(Estate_Machine *mach EINA_UNUSED)
{
   /* XXX Use mach allocator */
   return calloc(1, sizeof(Estate_Transition));
}

EAPI void
estate_transition_free(Estate_Transition *tr)
{
   estate_transition_deinit(tr);
   /* XXX Update when mach allocator will be used */
   free(tr);
}

EAPI Eina_Bool
estate_transition_init(Estate_Transition  *tr,
                       const char         *name,
                       const Estate_State *from,
                       const Estate_State *to,
                       Estate_Cb           func,
                       const char         *datakey)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(tr, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(from, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(to, EINA_FALSE);

   tr->name = eina_stringshare_add(name);
   tr->from = (Estate_State *)from;
   tr->to = (Estate_State *)to;
   tr->cb.func = func;
   tr->cb.data = NULL;
   tr->cb.key = (datakey) ? eina_stringshare_add(datakey) : NULL;

   return EINA_TRUE;
}

EAPI void
estate_transition_deinit(Estate_Transition *tr)
{
   if (tr->name) eina_stringshare_del(tr->name);
   if (tr->cb.key) eina_stringshare_del(tr->cb.key);
}

EAPI const char *
estate_transition_name_get(const Estate_Transition *st)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(st, NULL);
   return st->name;
}

