#include <Estate.h>
#include "bench.est.c"

static Estate_Machine *_fsm = NULL;

/* Use the prototype below to export the loading function */
Estate_Machine *estate_fsm_bench_load(void) EINA_MALLOC;

Estate_Machine *
estate_fsm_bench_load(void)
{
   return _estate_fsm_bench_load();
}

static void
_simple_benchmark_cb(int request EINA_UNUSED)
{
   estate_machine_transition_do(_fsm, "t1");
   estate_machine_transition_do(_fsm, "t2");
}

static void
_fail_benchmark_cb(int request EINA_UNUSED)
{
   estate_machine_transition_do(_fsm, "I_WILL_FAIL");
   estate_machine_transition_do(_fsm, "I_WILL_FAIL");
}

#if 0
static void
_big_benchmark_cb(int request EINA_UNUSED)
{
   unsigned int i;
   const unsigned int nb = 100000;

   for (i = 0; i < nb; ++i)
     {
        estate_machine_transition_do(_fsm, "t1");
        estate_machine_transition_do(_fsm, "t2");
        estate_machine_transition_do(_fsm, "t1");
        estate_machine_transition_do(_fsm, "t2");
        estate_machine_transition_do(_fsm, "t1");
        estate_machine_transition_do(_fsm, "t2");
        estate_machine_transition_do(_fsm, "t1");
        estate_machine_transition_do(_fsm, "t7");
        estate_machine_transition_do(_fsm, "t8");
        estate_machine_transition_do(_fsm, "t7");
        estate_machine_transition_do(_fsm, "t8");
        estate_machine_transition_do(_fsm, "t7");
        estate_machine_transition_do(_fsm, "t9");
        estate_machine_transition_do(_fsm, "t10");
        estate_machine_transition_do(_fsm, "t9");
        estate_machine_transition_do(_fsm, "t10");
        estate_machine_transition_do(_fsm, "t9");
        estate_machine_transition_do(_fsm, "t12");
        estate_machine_transition_do(_fsm, "t7");
        estate_machine_transition_do(_fsm, "t4"); /* Allows to loop on t1 */

        /* Below: will fail (no such transition) */
        estate_machine_transition_do(_fsm, "I_WILL_FAIL");
        estate_machine_transition_do(_fsm, "I_WILL_FAIL");
        estate_machine_transition_do(_fsm, "I_WILL_FAIL");
        estate_machine_transition_do(_fsm, "I_WILL_FAIL");
        estate_machine_transition_do(_fsm, "I_WILL_FAIL");
        estate_machine_transition_do(_fsm, "I_WILL_FAIL");
        estate_machine_transition_do(_fsm, "I_WILL_FAIL");
     }
}
#endif

int
main(void)
{
   Eina_Benchmark *bmk = NULL;

   /*=== Init ===*/
   estate_init();
   _fsm = estate_fsm_bench_load();
   EINA_SAFETY_ON_NULL_GOTO(_fsm, fail);
   bmk = eina_benchmark_new("perf", "run");
   EINA_SAFETY_ON_NULL_GOTO(bmk, fail);

   /* Tests will fail on purpose (to test worst cases). They
    * generate errors in eina_log.*/
   eina_log_domain_level_set("estate", EINA_LOG_LEVEL_CRITICAL);

//   eina_benchmark_register(bmk, "full", _big_benchmark_cb, 1, 100, 1);
   eina_benchmark_register(bmk, "simple", _simple_benchmark_cb, 1, 100, 1);
   eina_benchmark_register(bmk, "fail", _fail_benchmark_cb, 1, 100, 1);
   eina_benchmark_run(bmk);

   /*=== Shutdown ===*/
   estate_machine_free(_fsm);
   eina_benchmark_free(bmk);
   estate_shutdown();

   return 0;

fail:
   estate_machine_free(_fsm);
   eina_benchmark_free(bmk);
   estate_shutdown();
   return 1;
}

