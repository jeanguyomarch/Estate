#include "estate_cc.h"

typedef struct
{
   Eina_Hash *hash;
   Eina_Hash *states; /* Pointer only */
   Eina_Bool  ok;
   Eina_Stringshare *init;
   unsigned int init_count;
} Cache;


static Eina_Bool
_state_deduction_cb(const Eina_Hash *hash  EINA_UNUSED,
                    const void      *key   EINA_UNUSED,
                    void            *data,
                    void            *fdata)
{
   Cache *cache = fdata;
   Transit *t = data;
   Eina_Hash *states = cache->states;
   Eina_Stringshare *sh;
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
   from->check.from++;

   to = eina_hash_find(states, t->to);
   if (!to)
     {
        DBG("Deducing state [%s] (%s:to)", t->to, t->name);
        to = state_new(t->to, eina_stringshare_strlen(t->to));
        eina_hash_add(states, t->to, to);
     }
   to->check.to++;

   /* Check that there are NEVER two identical transitions. A transition is
    * identified by a <FROM:TO> string. If such a tuple has already been
    * added in the cached hash, then the transtion is a duplicate. */
   len = snprintf(buf, sizeof(buf), "%s:%s", t->from, t->to);
   sh = eina_stringshare_add_length(buf, len);
   if (eina_hash_find(cache->hash, sh))
     {
        CRI("Invalid transition: [%s]", buf);
        cache->ok = EINA_FALSE;
        return EINA_FALSE;
     }
   eina_hash_add(cache->hash, sh, t);
   return EINA_TRUE;
}

static Eina_Bool
_state_init_deduction_cb(const Eina_Hash *hash  EINA_UNUSED,
                         const void      *key   EINA_UNUSED,
                         void            *data,
                         void            *fdata)
{
   State *s = data;
   Cache *cache = fdata;

   if (s->check.to == 0)
     {
        cache->init_count++;
        cache->init = s->name;
     }
   return EINA_TRUE;

}

Eina_Bool
estate_cc_check_states(Eina_List *parse)
{
   Eina_List *l;
   Fsm *fsm;
   Cache cache;

   cache.hash = eina_hash_stringshared_new(NULL);

   EINA_LIST_FOREACH(parse, l, fsm)
     {
        cache.states = fsm->states;

        if (V(2)) DBG("Starting states deduction from transitions");
        cache.ok = EINA_TRUE;
        eina_hash_foreach(fsm->transitions, _state_deduction_cb, &cache);
        if (!cache.ok) break;

        /*=== Determine the init node, which must exists and be unique ===*/
        if (V(2)) DBG("Determining the init node");
        cache.init = NULL;
        cache.init_count = 0;
        eina_hash_foreach(fsm->states, _state_init_deduction_cb, &cache);

        /* Check for the uniqueness of the init node */
        if (cache.init_count > 1)
          {
             cache.ok = EINA_FALSE;
             ERR("The FSM topology allows %u nodes to be selected as initiators. "
                 "This is not allowed in FSM theory because some states will be "
                 "unreachable. Please check the topology of your FSM",
                 cache.init_count);
             break;
          }
        if (cache.init)
          {
             /* Check that @init has not been wrongly attributed */
             if ((fsm->init) && (cache.init != fsm->init))
               {
                  cache.ok = EINA_FALSE;
                  ERR("An init node has been deduced from the FSM (%s), but you have "
                      "provided a custom init node (%s). Consider removing the \"@init\" "
                      "property or check the topology of the FSM",
                      cache.init, fsm->init);
                  break;
               }
             else
               {
                  /* Register the init node */
                  fsm->init = eina_stringshare_add(cache.init);
               }
          }
        else
          {
             /* Is there an init node? */
             if (!fsm->init)
               {
                  cache.ok = EINA_FALSE;
                  ERR("No init node has been deduced from the FSM. "
                      "You need to enforce an init node with the \"@init\" property.");
                  break;
               }
          }
        if (V(2)) DBG("Init node: [%s]", fsm->init);
     }

   eina_hash_free(cache.hash);

   return cache.ok;
}

