#include "estate_private.h"

EAPI Estate_Machine *
estate_machine_new(unsigned int states,
                   unsigned int transitions)
{
   Estate_Machine *mach;

#if 0
   Eina_Mempool *mempool;
   size_t mem;
   mem = (states * sizeof(Estate_State)) +
      (transitions * sizeof(Estate_Transition)) +
      sizeof(Estate_Machine);
   mempool = eina_mempool_add("one_big", "Estate_Machine", NULL, mem);
#endif

   mach = malloc(sizeof(*mach));
   if (EINA_UNLIKELY(!mach))
     {
        CRI("Failed to allocate Estate_Machine");
        goto fail;
     }

   /* Pre-allocate states */
   mach->states = eina_inarray_new(sizeof(Estate_State), states);
   if (EINA_UNLIKELY(!mach->states))
     {
        CRI("Failed to allocate Estate_State array");
        goto fail;
     }

   /* Pre-allocate transitions */
   mach->transit = eina_inarray_new(sizeof(Estate_Transition), transitions);
   if (EINA_UNLIKELY(!mach->transit))
     {
        CRI("Failed to allocate Estate_Transition array");
        goto fail;
     }

   /* No current state */
   mach->current_state = NULL;

   return mach;

fail:
   estate_machine_free(mach);
   return NULL;
}

EAPI void
estate_machine_free(Estate_Machine *mach)
{
   if (!mach) return;
   eina_inarray_free(mach->states);
   eina_inarray_free(mach->transit);
   free(mach);
}

EAPI Eina_Bool
estate_machine_state_add(Estate_Machine     *mach,
                         const Estate_State *state)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(mach, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(state, EINA_FALSE);

   eina_inarray_push(mach->states, state);
   return EINA_TRUE;
}

EAPI Eina_Bool
estate_machine_transition_add(Estate_Machine          *mach,
                              const Estate_Transition *tr)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(mach, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(tr, EINA_FALSE);


   eina_inarray_push(mach->transit, tr);
   return EINA_TRUE;

}

EAPI Eina_Bool
estate_machine_check(Estate_Machine *mach)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(mach, EINA_FALSE);
   /* TODO Check */
   mach->current_state = eina_inarray_nth(mach->states, 0);
   return EINA_TRUE;
}

EAPI Estate_State *
estate_machine_current_state_get(const Estate_Machine *mach)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(mach, NULL);
   return mach->current_state;
}



#if 0
EAPI Eina_Bool
estate_ecore_main_loop_enabled_set(Estate_Machine *mach,
                                   Eina_Bool       set)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(mach, EINA_FALSE);
   unsigned int i;

   if (set)
     {
        if (mach->evt[0])
          return EINA_TRUE;
        else
          {
             mach->evt[0] = ecore_event_handler_add(ESTATE_EVENT_ENTERER, _deffer_cb, mach);
             mach->evt[1] = ecore_event_handler_add(ESTATE_EVENT_EXITER, _deffer_cb, mach);
             mach->evt[2] = ecore_event_handler_add(ESTATE_EVENT_TRANSITION, _deffer_cb, mach);
             /* TODO check allocs */
             return EINA_TRUE;
          }
     }
   else
     {
        if (mach->evt[0])
          {
             for (i = 0; i < EINA_C_ARRAY_LENGTH(mach->evt); ++i)
               ecore_event_handler_del(mach->evt[i]);
             return EINA_TRUE;
          }
        else
          return EINA_TRUE;
     }
}
#endif

static void
_do_transition(const Estate_State      *st EINA_UNUSED,
               const Estate_Transition *tr)
{
//   EINA_INLIST_FOREACH(st->cb[0]
   if (tr->cb.func) tr->cb.func(tr->cb.data,
                                ESTATE_CB_TYPE_TRANSITION, tr);
}

static Eina_Bool
_estate_state_transition_do(const Estate_State *st,
                            const char         *tr_name)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(st, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(tr_name, EINA_FALSE);

   unsigned int i;
   Eina_Array_Iterator iter;
   Estate_Transition *tr;

   EINA_ARRAY_ITER_NEXT(st->transit, i, tr, iter)
     {
        if (!strcmp(tr_name, tr->name))
          {
             _do_transition(st, tr);
             return EINA_TRUE;
          }
     }

   ERR("Nonexistant transition [%s] for state [%s]", tr_name, st->name);
   return EINA_FALSE;
}

EAPI Eina_Bool
estate_machine_transition_do(Estate_Machine *mach,
                             const char     *name)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(mach, EINA_FALSE);
   return _estate_state_transition_do(mach->current_state, name);
}

