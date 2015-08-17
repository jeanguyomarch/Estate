#ifndef _ESTATE_H_
#define _ESTATE_H_

#include <Eina.h>

typedef struct _Estate_Machine Estate_Machine;
typedef struct _Estate_State Estate_State;
typedef struct _Estate_Transition Estate_Transition;

typedef enum
{
   ESTATE_CB_TYPE_ENTERER         = 0,
   ESTATE_CB_TYPE_EXITER          = 1,
   ESTATE_CB_TYPE_TRANSITION      = 2
} Estate_Cb_Type;

typedef void (*Estate_Cb)(void                    *data,
                          Estate_Cb_Type           type,
                          const Estate_Transition *transition);


EAPI int estate_init(void);
EAPI int estate_shutdown(void);


EAPI Eina_Bool
estate_state_init(Estate_State             *st,
                  const char               *name,
                  const Estate_Transition **transitions,
                  unsigned int              transit_count);

EAPI void estate_state_deinit(Estate_State *st);

EAPI const char *estate_state_name_get(const Estate_State *st);

EAPI void
estate_state_cb_add(Estate_State   *st,
                    Estate_Cb_Type  cb_type,
                    Estate_Cb       func,
                    void           *data);

EAPI Eina_Bool
estate_transition_init(Estate_Transition  *tr,
                       const char         *name,
                       const Estate_State *from,
                       const Estate_State *to,
                       Estate_Cb           func,
                       void               *data);

EAPI void estate_transition_deinit(Estate_Transition *tr);

EAPI Estate_Machine *
estate_machine_new(unsigned int states,
                   unsigned int transitions);

EAPI void
estate_machine_free(Estate_Machine *mach);

EAPI Eina_Bool
estate_machine_state_add(Estate_Machine     *mach,
                         const Estate_State *state);

EAPI Eina_Bool
estate_machine_transition_add(Estate_Machine          *mach,
                              const Estate_Transition *tr);

EAPI Eina_Bool
estate_machine_check(Estate_Machine *mach);

EAPI Estate_State *estate_machine_current_state_get(const Estate_Machine *mach);


#define ESTATE_MACHINE_DECLARE(mach_) \
   static Estate_Machine _fsm_ ## mach_; \
   static Eina_Bool _estate_fsm_ ## mach_ ## _load(void)


//EAPI Eina_Bool estate_ecore_main_loop_enabled_set(Estate_Machine *mach, Eina_Bool set);

#endif /* ! _ESTATE_H_ */

