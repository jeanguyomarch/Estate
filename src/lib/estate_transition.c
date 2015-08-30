#include "estate_private.h"

EAPI Estate_Transition *
estate_transition_new(Estate_Machine *mach)
{
   return _estate_mempool_transition_push(mach->mempool);
}

EAPI void
estate_transition_free(Estate_Transition *tr)
{
   estate_transition_deinit(tr);
   /* Transition is freed by the machine */
}

EAPI Eina_Bool
estate_transition_init(Estate_Transition    *tr,
                       const char           *name,
                       const Estate_State   *from,
                       const Estate_State   *to,
                       const Estate_Cb_Ctor *cb,
                       const Estate_Cb_Ctor *st_enterer,
                       const Estate_Cb_Ctor *st_exiter)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(tr, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(from, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(to, EINA_FALSE);

   tr->name = eina_stringshare_add(name);
   tr->from = (Estate_State *)from;
   tr->to = (Estate_State *)to;

   /* Set transition callback */
   if (cb)
     {
        tr->cb.func = cb->func;
        tr->cb.key = (cb->key) ?
           eina_stringshare_add_length(cb->key, cb->key_len) : NULL;
     }

   /* Set state enterer callback */
   if (st_enterer)
     {
        tr->st_enterer.func = st_enterer->func;
        tr->st_enterer.key = (st_enterer->key) ?
           eina_stringshare_add_length(st_enterer->key, st_enterer->key_len) : NULL;
        tr->st_enterer.result = ESTATE_CB_OK;
     }

   /* Set state exiter callback */
   if (st_exiter)
     {
        tr->st_exiter.func = st_exiter->func;
        tr->st_exiter.key = (st_exiter->key) ?
           eina_stringshare_add_length(st_exiter->key, st_enterer->key_len) : NULL;
        tr->st_exiter.result = ESTATE_CB_OK;
     }

   return EINA_TRUE;
}

EAPI void
estate_transition_deinit(Estate_Transition *tr)
{
   if (tr->name) eina_stringshare_del(tr->name);
   if (tr->cb.key) eina_stringshare_del(tr->cb.key);
   if (tr->st_enterer.key) eina_stringshare_del(tr->st_enterer.key);
   if (tr->st_exiter.key) eina_stringshare_del(tr->st_exiter.key);
}

EAPI const char *
estate_transition_name_get(const Estate_Transition *st)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(st, NULL);
   return st->name;
}

