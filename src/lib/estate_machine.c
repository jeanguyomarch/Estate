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
   mempool = _estate_mempool_new(mem, EINA_FALSE);
#endif

   mach = malloc(sizeof(*mach));
   if (EINA_UNLIKELY(!mach))
     {
        CRI("Failed to allocate Estate_Machine");
        goto fail;
     }

   /* Pre-allocate states */
   mach->states = eina_array_new(states);
   if (EINA_UNLIKELY(!mach->states))
     {
        CRI("Failed to allocate Estate_State array");
        goto fail;
     }

   /* Pre-allocate transitions */
   mach->transit = eina_array_new(transitions);
   if (EINA_UNLIKELY(!mach->transit))
     {
        CRI("Failed to allocate Estate_Transition array");
        goto fail;
     }

   /* Create hash tha will contain data */
   mach->data = eina_hash_stringshared_new(NULL);
   if (EINA_UNLIKELY(!mach->data))
     {
        CRI("Failed to create Eina_Hash");
        goto fail;
     }

   /* No current state */
   mach->current_state = NULL;
   mach->locked = EINA_FALSE;

   return mach;

fail:
   estate_machine_free(mach);
   return NULL;
}

EAPI void
estate_machine_free(Estate_Machine *mach)
{
   if (!mach) return;
   if (mach->data) eina_hash_free(mach->data);
   if (mach->states) eina_array_free(mach->states);
   if (mach->transit) eina_array_free(mach->transit);
   free(mach);
}

EAPI Eina_Bool
estate_machine_state_add(Estate_Machine     *mach,
                         const Estate_State *state)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(mach, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(state, EINA_FALSE);

   if (EINA_UNLIKELY(mach->locked))
     {
        ERR("Attempt to add a new state but the state machine is locked");
        return EINA_FALSE;
     }

   eina_array_push(mach->states, state);
   return EINA_TRUE;
}

EAPI Eina_Bool
estate_machine_transition_add(Estate_Machine          *mach,
                              const Estate_Transition *tr)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(mach, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(tr, EINA_FALSE);

   if (EINA_UNLIKELY(mach->locked))
     {
        ERR("Attempt to add a new transition but the state machine is locked");
        return EINA_FALSE;
     }

   eina_array_push(mach->transit, tr);
   return EINA_TRUE;

}

EAPI Estate_State *
estate_machine_current_state_get(const Estate_Machine *mach)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(mach, NULL);
   if (EINA_UNLIKELY(!mach->locked))
     {
        ERR("Attempt to get current state of a state machine, but it is not"
            " locked");
        return NULL;
     }
   return mach->current_state;
}

EAPI Eina_Bool
estate_machine_data_set(Estate_Machine *mach,
                        const char     *key,
                        const void     *value)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(mach,  EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(key,   EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(value, EINA_FALSE);

   Eina_Stringshare *shr;

   if (EINA_UNLIKELY(!mach->locked))
     {
        ERR("Attempt to set data of a state machine, but it is not locked");
        return EINA_FALSE;
     }

   shr = eina_stringshare_add(key);
   if (eina_hash_find(mach->data, shr) != NULL)
     {
        ERR("You required to register data \"%s\", but it is already "
            "reserverd", shr);
        goto fail;
     }

   if (!eina_hash_add(mach->data, shr, value))
     {
        ERR("Failed to register data \"%s\" for value <%p>", shr, value);
        goto fail;
     }

   return EINA_TRUE;

fail:
   eina_stringshare_del(shr);
   return EINA_FALSE;
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

#if 0
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
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, EINA_FALSE);
   if (EINA_UNLIKELY(!mach->locked))
     {
        ERR("Attempt to do a transition but the state machine is not locked");
        return EINA_FALSE;
     }
   return _estate_state_transition_do(mach->current_state, name);
}
#endif

EAPI void
estate_machine_lock(Estate_Machine     *mach,
                    const Estate_State *current)
{
   EINA_SAFETY_ON_NULL_RETURN(mach);
   EINA_SAFETY_ON_NULL_RETURN(current);

   mach->current_state = (Estate_State *)current;
   mach->locked = EINA_TRUE;
}

EAPI Eina_Bool
estate_machine_transition_do(Estate_Machine *mach,
                             const char     *transition)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(mach, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(transition, EINA_FALSE);

   Estate_Transition *tr;
   Eina_Stringshare *shr;
   Eina_Array_Iterator iter;
   unsigned int i;
   Eina_Bool found = EINA_FALSE;

   /* For immediate search */
   shr = eina_stringshare_add(transition);

   /* Find transition */
   EINA_ARRAY_ITER_NEXT(mach->current_state->transit, i, tr, iter)
     {
        /* Stringshares... I can do that */
        if (tr->name == shr)
          {
             found = EINA_TRUE;
             break;
          }
     }

   /* Gracefully fail if no transition is available */
   if (!found)
     {
        ERR("Could not find transition \"%s\" at current state (%s)",
            transition, mach->current_state->name);
        goto fail;
     }

   /* Safety check: is the transition really coming from the current state?
    * This should never fail when the machine is generated by estate_cc. */
   if (EINA_UNLIKELY(tr->from != mach->current_state))
     {
        CRI("Wtf?! Transition \"%s\" is inconsistent", tr->name);
        goto fail;
     }

   /* Call exiter callback of current state */
   _estate_state_cb_call(mach, tr->from, tr, ESTATE_CB_TYPE_EXITER);

   /* Execute the transition callback */
   if (tr->cb.func)
     {
        _estate_misc_cb_cache(mach, &tr->cb);
        tr->cb.func(tr->cb.data, ESTATE_CB_TYPE_TRANSITION, tr);
     }

   /* Call enterer callback of next state */
   _estate_state_cb_call(mach, tr->to, tr, ESTATE_CB_TYPE_ENTERER);

   /* Update current state */
   mach->current_state = tr->to;

   eina_stringshare_del(shr);
   return EINA_TRUE;

fail:
   if (shr) eina_stringshare_del(shr);
   return EINA_FALSE;
}


