#include <Estate.h>

ESTATE_MACHINE_DECLARE(turnstile);

Eina_Bool estate_fsm_turnstile_load(void);

Eina_Bool
estate_fsm_turnstile_load(void)
{
   const Eina_Bool ok = _estate_fsm_turnstile_load();
   return ok;
}

static void
_push(void                    *data,
      Estate_Cb_Type           type,
      const Estate_Transition *transition)
{

}

static void
_push_locked(void                    *data,
             Estate_Cb_Type           type,
             const Estate_Transition *transition)
{

}

static void
_coin(void                    *data,
      Estate_Cb_Type           type,
      const Estate_Transition *transition)
{

}

static void
_coin_unlocked(void                    *data,
               Estate_Cb_Type           type,
               const Estate_Transition *transition)
{

}

#include "turnstile.est.c"

