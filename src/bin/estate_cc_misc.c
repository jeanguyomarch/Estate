#include "estate_cc.h"

State *
state_new(const char *name,
          const int   len)
{
   State *s;
   s = calloc(1, sizeof(*s));
   s->name = eina_stringshare_add_length(name, len);
   return s;
}

void
state_free(State *s)
{
   if (s->enterer.func) eina_stringshare_del(s->enterer.func);
   if (s->enterer.data) eina_stringshare_del(s->enterer.data);
   if (s->exiter.func) eina_stringshare_del(s->exiter.func);
   if (s->exiter.data) eina_stringshare_del(s->exiter.data);
   eina_stringshare_del(s->name);
   if (s->trs) free(s->trs);
   free(s);
}

Transit *
transit_new(const char *name,
            const int   len)
{
   Transit *t;
   t = calloc(1, sizeof(*t));
   t->name = eina_stringshare_add_length(name, len);
   return t;
}

void
transit_free(Transit *t)
{
   eina_stringshare_del(t->name);
   eina_stringshare_del(t->from);
   eina_stringshare_del(t->to);
   if (t->cb.func) eina_stringshare_del(t->cb.func);
   if (t->cb.data) eina_stringshare_del(t->cb.data);
   free(t);
}

Fsm *
fsm_new(const char *name,
        const int   len)
{
   /* TODO error cases */
   Fsm *f;
   f = calloc(1, sizeof(*f));
   f->name = eina_stringshare_add_length(name, len);
   f->transitions = eina_hash_stringshared_new(EINA_FREE_CB(transit_free));
   f->states = eina_hash_stringshared_new(EINA_FREE_CB(state_free));

   return f;
}

void
fsm_free(Fsm *f)
{
   if (!f) return;
   eina_stringshare_del(f->name);
   eina_hash_free(f->transitions);
   eina_hash_free(f->states);
   free(f);
}

