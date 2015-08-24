#include <Estate.h>
#include "turnstile.est.c"

/* Use the prototype below to export the loading function */
Estate_Machine *estate_fsm_turnstile_load(void) EINA_MALLOC;

Estate_Machine *
estate_fsm_turnstile_load(void)
{
   return _estate_fsm_turnstile_load();
}

static void
_push(void                    *data       EINA_UNUSED,
      Estate_Cb_Type           type       EINA_UNUSED,
      const Estate_Transition *transition EINA_UNUSED)
{
   printf("TRANSITION: <%s>\n", estate_transition_name_get(transition));
}

static void
_push_locked(void                    *data       EINA_UNUSED,
             Estate_Cb_Type           type       EINA_UNUSED,
             const Estate_Transition *transition)
{
   printf("TRANSITION: <%s>\n", estate_transition_name_get(transition));
}

static void
_coin(void                    *data       EINA_UNUSED,
      Estate_Cb_Type           type       EINA_UNUSED,
      const Estate_Transition *transition)
{
   printf("TRANSITION: <%s>\n", estate_transition_name_get(transition));
}

static void
_coin_unlocked(void                    *data       EINA_UNUSED,
               Estate_Cb_Type           type       EINA_UNUSED,
               const Estate_Transition *transition)
{
   printf("TRANSITION: <%s>\n", estate_transition_name_get(transition));
}

static void
_locked_in(void                    *data       EINA_UNUSED,
           Estate_Cb_Type           type       EINA_UNUSED,
           const Estate_Transition *transition EINA_UNUSED)
{
   printf("STATE <ENTERER> I am now locked\n");
}

static void
_locked_out(void                    *data       EINA_UNUSED,
            Estate_Cb_Type           type       EINA_UNUSED,
            const Estate_Transition *transition EINA_UNUSED)
{
   printf("STATE <EXITER> I am not locked anymore\n");
}

static void
_unlocked_in(void                    *data       EINA_UNUSED,
             Estate_Cb_Type           type       EINA_UNUSED,
             const Estate_Transition *transition EINA_UNUSED)
{
   printf("STATE <ENTERER> I am now unlocked\n");
}

static void
_unlocked_out(void                    *data       EINA_UNUSED,
              Estate_Cb_Type           type       EINA_UNUSED,
              const Estate_Transition *transition EINA_UNUSED)
{
   printf("STATE <EXITER> I am not unlocked anymore\n");
}

int
main(void)
{
   Estate_Machine *fsm;

   fsm = estate_fsm_turnstile_load();
   estate_machine_transition_do(fsm, "push");
   return 0;
}

