#include "estate_private.h"

Estate_Mempool *
_estate_mempool_new(unsigned int states,
                    unsigned int transitions,
                    Eina_Bool    zero)
{
   EINA_SAFETY_ON_TRUE_RETURN_VAL(states == 0, NULL);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(transitions == 0, NULL);

   Estate_Mempool *mp;
   size_t mem;
   size_t mem_trs;
   size_t mem_states;

   mem_trs = sizeof(Estate_Transition) * transitions;
   mem_states = sizeof(Estate_State) * states;
   mem = sizeof(Estate_Mempool) + sizeof(Estate_Machine) +
      mem_states + mem_trs;

   mp = malloc(mem);
   if (EINA_UNLIKELY(!mp))
     {
        CRI("Failed to create mempool (size: %zu)", mem);
        return NULL;
     }

   mp->base = (unsigned char *)mp + sizeof(Estate_Mempool);
   mp->stack_states = mp->base + sizeof(Estate_Machine);
   mp->stack_transits = mp->stack_states + mem_states;
   mp->end = (unsigned char *)mp + mem;
   mp->states_reg = 0;
   mp->transit_reg = 0;

   if (zero)
     memset(mp->base, 0, mem - sizeof(Estate_Mempool));

   return mp;
}

void
_estate_mempool_free(Estate_Mempool *mp)
{
   unsigned int i;

   /* Release all transitions */
   for (i = 0; i < mp->transit_reg; ++i)
     {
        mp->stack_transits -= sizeof(Estate_Transition);
        estate_transition_free((Estate_Transition *)mp->stack_transits);
     }

   /* Release all states */
   for (i = 0; i < mp->states_reg; ++i)
     {
        mp->stack_states -= sizeof(Estate_State);
        estate_state_free((Estate_State *)mp->stack_states);
     }

   free(mp);
}

Estate_State *
_estate_mempool_state_push(Estate_Mempool *mp)
{
   void *ptr;

   ptr = mp->stack_states;
   mp->stack_states += sizeof(Estate_State);
   if (EINA_UNLIKELY(mp->stack_states > mp->stack_transits))
     {
        CRI("Mempool overflowed while trying to push a state");
        return NULL;
     }
   return ptr;
}

Estate_Transition *
_estate_mempool_transition_push(Estate_Mempool *mp)
{
   void *ptr;

   ptr = mp->stack_transits;
   mp->stack_transits += sizeof(Estate_Transition);
   if (EINA_UNLIKELY(mp->stack_transits > mp->end))
     {
        CRI("Mempool overflowed while trying to push a transition");
        return NULL;
     }
   return ptr;
}

