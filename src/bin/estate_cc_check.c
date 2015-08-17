#include "estate_cc.h"

static Eina_Bool
_state_deduction_cb(const Eina_Hash *hash  EINA_UNUSED,
                    const void      *key   EINA_UNUSED,
                    void            *data,
                    void            *fdata)
{
   Eina_Hash *states = fdata;
   Transit *t = data;
   State *s;

   if (!eina_hash_find(states, t->from))
     {
        DBG("Deducing state [%s] (%s:from)", t->from, t->name);
        s = state_new(t->from, eina_stringshare_strlen(t->from));
        eina_hash_add(states, t->from, s);
     }

   if (!eina_hash_find(states, t->to))
     {
        DBG("Deducing state [%s] (%s:to)", t->to, t->name);
        s = state_new(t->to, eina_stringshare_strlen(t->to));
        eina_hash_add(states, t->to, s);
     }

   return EINA_TRUE;
}

Eina_Bool
estate_cc_check_states(Eina_List *parse)
{
   Eina_List *l;
   Fsm *fsm;

   EINA_LIST_FOREACH(parse, l, fsm)
     {
        if (V(2)) DBG("Starting states deduction from transitions");
        eina_hash_foreach(fsm->transitions, _state_deduction_cb, fsm->states);
     }

   return EINA_TRUE;
}

