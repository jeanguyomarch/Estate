#include <Estate.h>
#include "turnstile.est.c"

/* Use the prototype below to export the loading function */
Estate_Machine *estate_fsm_turnstile_load(void) EINA_MALLOC;

Estate_Machine *
estate_fsm_turnstile_load(void)
{
   return _estate_fsm_turnstile_load();
}

static int
_push(void                    *data       EINA_UNUSED,
      Estate_Cb_Type           type       EINA_UNUSED,
      const Estate_Transition *transition EINA_UNUSED)
{
   printf("TRANSITION: <%s>\n", estate_transition_name_get(transition));
   return ESTATE_CB_OK;
}

static int
_push_locked(void                    *data       EINA_UNUSED,
             Estate_Cb_Type           type       EINA_UNUSED,
             const Estate_Transition *transition)
{
   printf("TRANSITION: <%s>\n", estate_transition_name_get(transition));
   return 42; /* To try this out */
}

static int
_coin(void                    *data       EINA_UNUSED,
      Estate_Cb_Type           type       EINA_UNUSED,
      const Estate_Transition *transition)
{
   printf("TRANSITION: <%s>\n", estate_transition_name_get(transition));
   return ESTATE_CB_OK;
}

static int
_coin_unlocked(void                    *data       EINA_UNUSED,
               Estate_Cb_Type           type       EINA_UNUSED,
               const Estate_Transition *transition)
{
   printf("TRANSITION: <%s>\n", estate_transition_name_get(transition));
   return ESTATE_CB_OK;
}

static int
_locked_in(void                    *data       EINA_UNUSED,
           Estate_Cb_Type           type       EINA_UNUSED,
           const Estate_Transition *transition EINA_UNUSED)
{
   printf("STATE <ENTERER> I am now locked\n");
   return ESTATE_CB_OK;
}

static int
_locked_out(void                    *data       EINA_UNUSED,
            Estate_Cb_Type           type       EINA_UNUSED,
            const Estate_Transition *transition EINA_UNUSED)
{
   printf("STATE <EXITER> I am not locked anymore\n");
   return ESTATE_CB_OK;
}

static int
_unlocked_in(void                    *data       EINA_UNUSED,
             Estate_Cb_Type           type       EINA_UNUSED,
             const Estate_Transition *transition EINA_UNUSED)
{
   printf("STATE <ENTERER> I am now unlocked\n");
   return ESTATE_CB_OK;
}

static int
_unlocked_out(void                    *data,
              Estate_Cb_Type           type       EINA_UNUSED,
              const Estate_Transition *transition EINA_UNUSED)
{
   int *mydata = data;
   printf("STATE <EXITER> I am not unlocked anymore, and my data is <%i>\n",
          *mydata);
   return ESTATE_CB_OK;
}

int
main(void)
{
   Estate_Machine *fsm;
   static int mydata = 777;

   estate_init();


   fsm = estate_fsm_turnstile_load();

   estate_machine_data_set(fsm, "exiter_datakey", &mydata);

   estate_machine_transition_do(fsm, "push_locked");
   printf("RETURN: from TRANSITION CALLBACK: %i\n",
          estate_machine_cb_check(fsm, ESTATE_CB_TYPE_TRANSITION));
   estate_machine_transition_do(fsm, "coin");
   estate_machine_transition_do(fsm, "coin_unlocked");
   estate_machine_transition_do(fsm, "coin_unlocked");
   estate_machine_transition_do(fsm, "push");
   estate_machine_transition_do(fsm, "push"); /* Fails (expected) */

   estate_machine_free(fsm);

   estate_shutdown();
   return 0;
}

