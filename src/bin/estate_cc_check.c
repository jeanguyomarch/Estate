#include "estate_cc.h"

static Eina_Hash *_cache = NULL;

static inline void
_cache_flush(void)
{
   eina_hash_free_buckets(_cache);
}

static Eina_Bool
_cache_add(const char         *str,
           const unsigned int  len,
           Transit            *t)
{
   Eina_Stringshare *sh;
   sh = eina_stringshare_add_length(str, len);
   if (eina_hash_find(_cache, sh))
     return EINA_FALSE;
   eina_hash_add(_cache, sh, t);
   return EINA_TRUE;
}

static Eina_Bool
_state_deduction_cb(const Eina_Hash *hash  EINA_UNUSED,
                    const void      *key   EINA_UNUSED,
                    void            *data,
                    void            *fdata)
{
   Eina_Hash *states = fdata;
   Transit *t = data;
   State *from, *to;
   char buf[2048];
   unsigned int len;

   from = eina_hash_find(states, t->from);
   if (!from)
     {
        DBG("Deducing state [%s] (%s:from)", t->from, t->name);
        from = state_new(t->from, eina_stringshare_strlen(t->from));
        eina_hash_add(states, t->from, from);
     }

   to = eina_hash_find(states, t->to);
   if (!to)
     {
        DBG("Deducing state [%s] (%s:to)", t->to, t->name);
        to = state_new(t->to, eina_stringshare_strlen(t->to));
        eina_hash_add(states, t->to, to);
     }

   len = snprintf(buf, sizeof(buf), "%s%s", t->from, t->to);
   if (!_cache_add(buf, len, t))
     {
        CRI("WTF!? [%s] failed", buf);
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
        _cache_flush();
        eina_hash_foreach(fsm->transitions, _state_deduction_cb, fsm->states);
     }

   return EINA_TRUE;
}

Eina_Bool
estate_cc_check_init(void)
{
   _cache = eina_hash_stringshared_new(NULL);
   if (EINA_UNLIKELY(!_cache))
     {
        CRI("Failed to allocate cache");
        return EINA_FALSE;
     }
   return EINA_TRUE;
}

void
estate_cc_check_shutdown(void)
{
   eina_hash_free(_cache);
}

