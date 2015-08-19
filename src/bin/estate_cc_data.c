#include "estate_cc.h"

static Eina_Bool
_each_transit_print_cb(const Eina_Hash *hash  EINA_UNUSED,
                       const void      *key   EINA_UNUSED,
                       void            *data,
                       void            *fdata EINA_UNUSED)
{
   Transit *t = data;
   printf("++ TRANSITION: [%s] : [%s] -> [%s]\n"
          "+++ transition: [%s], [%s]\n",
          t->name, t->from, t->to,
          t->cb.func, t->cb.data);

   return EINA_TRUE;
}

static Eina_Bool
_each_state_print_cb(const Eina_Hash *hash  EINA_UNUSED,
                     const void      *key   EINA_UNUSED,
                     void            *data,
                     void            *fdata EINA_UNUSED)
{
   State *s = data;
   printf("++ STATE: [%s]\n"
          "+++ enterer: [%s], [%s]\n"
          "+++ exiter: [%s], [%s]\n",
          s->name,
          s->enterer.func, s->enterer.data,
          s->exiter.func, s->exiter.data);

   return EINA_TRUE;
}

void
estate_cc_data_print(Eina_List *fsm)
{
   Eina_List *l;
   Fsm *f;

   EINA_LIST_FOREACH(fsm, l, f)
     {
        printf("+ FSM: [%s]\n", f->name);
        eina_hash_foreach(f->transitions, _each_transit_print_cb, NULL);
        eina_hash_foreach(f->states, _each_state_print_cb, NULL);
     }
}

