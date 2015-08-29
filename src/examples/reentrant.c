#include <Estate.h>
#include "reentrant.est.c"

static unsigned int _count = 0;
static Estate_Machine *_fsm = NULL;

/* Use the prototype below to export the loading function */
Estate_Machine *estate_fsm_fsm_load(void) EINA_MALLOC;

Estate_Machine *
estate_fsm_fsm_load(void)
{
   return _estate_fsm_fsm_load();
}

static int
_forward(void                    *data       EINA_UNUSED,
         Estate_Cb_Type           type       EINA_UNUSED,
         const Estate_Transition *transition)
{
   const char *tr = estate_transition_name_get(transition);

   if (_count > 20) return ESTATE_CB_OK;

   printf("[%u] Entering current state <%s>... forwarding...\n",
          ++_count,
          estate_state_name_get(estate_machine_current_state_get(_fsm)));


   if (!strcmp(tr, "t1"))
     estate_machine_transition_do(_fsm, "t2");
   else if (!strcmp(tr, "t2"))
     estate_machine_transition_do(_fsm, "t3");
   else /* t3 */
     estate_machine_transition_do(_fsm, "t1");

   return ESTATE_CB_OK;
}

static int
_transit_cb(void                    *data       EINA_UNUSED,
            Estate_Cb_Type           type       EINA_UNUSED,
            const Estate_Transition *transition)
{
   printf("Doing transition <%s>\n",
          estate_transition_name_get(transition));
   return ESTATE_CB_OK;
}


int
main(void)
{
   estate_init();

   _fsm = estate_fsm_fsm_load();

   estate_machine_transition_do(_fsm, "t1");

   estate_machine_free(_fsm);

   estate_shutdown();

   return 0;
}
