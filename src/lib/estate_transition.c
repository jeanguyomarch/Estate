#include "estate_private.h"

EAPI Eina_Bool
estate_transition_init(Estate_Transition  *tr,
                       const char         *name,
                       const Estate_State *from,
                       const Estate_State *to,
                       Estate_Cb           func,
                       void               *data)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(from, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(to, EINA_FALSE);

   tr->name = name;
   tr->from = (Estate_State *)from;
   tr->to = (Estate_State *)to;
   tr->cb.func = func;
   tr->cb.data = data;

   return EINA_TRUE;
}

EAPI void
estate_transition_deinit(Estate_Transition *tr EINA_UNUSED)
{
   /* Nothing to do... */
}

EAPI void
estate_transition_do(const Estate_Transition *tr)
{
   EINA_SAFETY_ON_NULL_RETURN(tr);
   if (tr->cb.func)
     tr->cb.func(tr->cb.data, ESTATE_CB_TYPE_TRANSITION, (Estate_Transition *)tr);
}

