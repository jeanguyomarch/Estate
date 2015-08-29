#include "estate_private.h"

static int
_compare_cb(const void *key,
            const void *data)
{
   Eina_Stringshare *shr = key;
   const Estate_Transition *t =
      (const Estate_Transition *)(*(Estate_Transition * const *)data);

   return (int)(shr - t->name);
}


EAPI Estate_Machine *
estate_machine_new(unsigned int states,
                   unsigned int transitions)
{
   Estate_Machine *mach = NULL;
   Estate_Mempool *mempool;

   mempool = _estate_mempool_new(states, transitions, EINA_TRUE);
   if (EINA_UNLIKELY(!mempool))
     {
        CRI("Failed to create mempool");
        goto fail;
     }

   /* Retrieve the machine and attach the mempool */
   mach = (Estate_Machine *)mempool->base;
   mach->mempool = mempool;
   mach->states_count = states;
   mach->transit_count = transitions;

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
   mach->in_cb = CB_UNLOCKED;

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
   _estate_mempool_free(mach->mempool);
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

   mach->mempool->states_reg++;
   if (EINA_UNLIKELY(mach->mempool->states_reg > mach->states_count))
     {
        CRI("You tried to add a state, but the amount of pre-registered "
            "states has been reached");
        return EINA_FALSE;
     }

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

  mach->mempool->transit_reg++;
   if (EINA_UNLIKELY(mach->mempool->transit_reg > mach->transit_count))
     {
        CRI("You tried to add a transition, but the amount of pre-registered "
            "transitions has been reached");
        return EINA_FALSE;
     }

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

EAPI void
estate_machine_lock(Estate_Machine     *mach,
                    const Estate_State *current)
{
   EINA_SAFETY_ON_NULL_RETURN(mach);
   EINA_SAFETY_ON_NULL_RETURN(current);

   /* Safety: was the machine complete? */
   if (EINA_UNLIKELY(mach->mempool->states_reg != mach->states_count))
     {
        CRI("You have registered %u states, but only %u have been added",
            mach->states_count, mach->mempool->states_reg);
        return;
     }
   if (EINA_UNLIKELY(mach->mempool->transit_reg != mach->transit_count))
     {
        CRI("You have registered %u transitions, but only %u have been added",
            mach->transit_count, mach->mempool->transit_reg);
        return;
     }

   mach->current_state = (Estate_State *)current;
   mach->locked = EINA_TRUE;
}

EAPI Eina_Bool
estate_machine_transition_do(Estate_Machine *mach,
                             const char     *transition)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(mach, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(transition, EINA_FALSE);

   Estate_Transition *tr = NULL, **ptr;
   Eina_Stringshare *shr;
   Eina_List *deferred;

   if (EINA_UNLIKELY(!mach->locked))
     {
        ERR("Attempt to do a transition, but the state machine is not locked");
        return EINA_FALSE;
     }

   /* Deffer transition if this function is called in a callback context */
   if (mach->in_cb != CB_UNLOCKED)
     {
        DBG("Transition is called in a callback context. "
            "Deferring transition %s after the entering callback...",
            transition);
        mach->cb_defer = eina_list_append(mach->cb_defer, transition);
        return EINA_TRUE;
     }

   /* For immediate search */
   shr = eina_stringshare_add(transition);

   /* Binary search (this is why the transitions array must be sorted at init */
   ptr = bsearch(shr, mach->current_state->transit,
                 mach->current_state->transit_count, sizeof(Estate_Transition *),
                 _compare_cb);
   if (ptr)
     tr = *ptr;

   /* Gracefully fail if no transition is available */
   if (!tr)
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

   /* Update current transition */
   mach->current_transition = tr;

   /* Call exiter callback of current state */
   _estate_state_cb_call(mach, tr->from, tr, ESTATE_CB_TYPE_EXITER);

   /* Execute the transition callback */
   _estate_misc_cb_call(mach, &tr->cb, ESTATE_CB_TYPE_TRANSITION, tr);

   /* We just jumped in the new state... Update the current state
    * before executing the entering callback */
   mach->current_state = tr->to;

   /* Call enterer callback of next state */
   _estate_state_cb_call(mach, tr->to, tr, ESTATE_CB_TYPE_ENTERER);

   /* Release... */
   eina_stringshare_del(shr);

   /* Allow the state machine to host another cb deferring */
   deferred = mach->cb_defer;
   mach->cb_defer = NULL;

   EINA_LIST_FREE(deferred, transition)
     {
        estate_machine_transition_do(mach, transition);
     }

   return EINA_TRUE;

fail:
   if (shr) eina_stringshare_del(shr);
   return EINA_FALSE;
}

EAPI int
estate_machine_cb_check(const Estate_Machine *mach,
                        Estate_Cb_Type        type)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(mach, INT_MIN);

   if (EINA_UNLIKELY(!mach->locked))
     {
        ERR("Attempt to check for a callbakc, but the state machine not locked");
        return EINA_FALSE;
     }

   switch (type)
     {
      case ESTATE_CB_TYPE_ENTERER:
         return mach->current_state->cb[type].result;

      case ESTATE_CB_TYPE_TRANSITION:
         return mach->current_transition->cb.result;

      case ESTATE_CB_TYPE_EXITER:
         return mach->current_transition->from->cb[type].result;
     }

   /* Should not happen... */
   return INT_MIN;
}

