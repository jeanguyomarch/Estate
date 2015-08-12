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
   mach->states = malloc(sizeof(Estate_State) * states);
   if (EINA_UNLIKELY(!mach->states))
     {
        CRI("Failed to allocate Estate_State array");
        goto fail;
     }
   mach->states_count = states;
   mach->st_curr = 0;

   /* Pre-allocate transitions */
   mach->transit = malloc(sizeof(Estate_Transition) * transitions);
   if (EINA_UNLIKELY(!mach->transit))
     {
        CRI("Failed to allocate Estate_Transition array");
        goto fail;
     }
   mach->transit_count = transitions;
   mach->tr_curr = 0;

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
   free(mach->states);
   free(mach->transit);
   free(mach);
}

EAPI Eina_Bool
estate_machine_state_add(Estate_Machine     *mach,
                         const Estate_State *state)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(mach, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(state, EINA_FALSE);

   if (mach->st_curr >= mach->states_count)
     {
        CRI("You tried to add state #%u but max is %u",
            mach->st_curr, mach->states_count);
        return EINA_FALSE;
     }

   memcpy(&(mach->states[mach->st_curr]), state, sizeof(Estate_State));
   mach->st_curr++;
   return EINA_TRUE;
}

EAPI Eina_Bool
estate_machine_transition_add(Estate_Machine          *mach,
                              const Estate_Transition *tr)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(mach, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(tr, EINA_FALSE);

   if (mach->tr_curr >= mach->transit_count)
     {
        CRI("You tried to add state #%u but max is %u",
            mach->tr_curr, mach->transit_count);
        return EINA_FALSE;
     }

   memcpy(&(mach->transit[mach->tr_curr]), tr, sizeof(Estate_Transition));
   mach->tr_curr++;
   return EINA_TRUE;

}

EAPI Eina_Bool
estate_machine_check(Estate_Machine *mach)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(mach, EINA_FALSE);
   /* TODO Check */
   mach->current_state = &(mach->states[0]);
   return EINA_TRUE;
}

