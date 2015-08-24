/**
 * @file Estate.h
 * @author Jean Guyomarc'h <jean.guyomarch@gmail.com>
 * @brief The Estate API documentation
 *
 * Estate is a library to create and manipulate finite
 * state machines (FSM).
 */

#ifndef _ESTATE_H_
#define _ESTATE_H_

#include <Eina.h>



typedef struct _Estate_Machine Estate_Machine;
typedef struct _Estate_State Estate_State;
typedef struct _Estate_Transition Estate_Transition;

/**
 * @defgroup Estate_Main Top level functions
 * API that affect Estate as a whole
 * @{
 */

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

/**
 * @}
 */



/**
 * @defgroup Estate_State State API
 * API to manipulate States in an FSM
 * @{
 */

EAPI Estate_State *estate_state_new(Estate_Machine *mach);
EAPI void estate_state_free(Estate_State *st);

EAPI Eina_Bool
estate_state_init(Estate_State             *st,
                  const char               *name,
                  const Estate_Transition **transitions,
                  unsigned int              transit_count,
                  Estate_Cb                 enterer,
                  const char               *enterer_datakey,
                  Estate_Cb                 exiter,
                  const char               *exiter_datakey);

EAPI void estate_state_deinit(Estate_State *st);

EAPI const char *estate_state_name_get(const Estate_State *st);

/**
 * @}
 */


/**
 * @defgroup Estate_Transition Transition API
 * API to manipulate Transitions in an FSM
 * @{
 */

EAPI Estate_Transition *estate_transition_new(Estate_Machine *mach);
EAPI void estate_transition_free(Estate_Transition *tr);

EAPI Eina_Bool
estate_transition_init(Estate_Transition  *tr,
                       const char         *name,
                       const Estate_State *from,
                       const Estate_State *to,
                       Estate_Cb           func,
                       void               *data);

EAPI void estate_transition_deinit(Estate_Transition *tr);

EAPI const char *estate_transition_name_get(const Estate_Transition *st);

/**
 * @}
 */


/**
 * @defgroup Estate_Machine Finite State Machine API
 * API to manipulate finite state machines objects
 * @{
 */

EAPI Estate_Machine *
estate_machine_new(unsigned int states,
                   unsigned int transitions);

EAPI void
estate_machine_free(Estate_Machine *mach);

EAPI Eina_Bool
estate_machine_data_set(Estate_Machine *mach,
                        const char     *key,
                        const void     *value);

EAPI Eina_Bool
estate_machine_state_add(Estate_Machine     *mach,
                         const Estate_State *state);

EAPI Eina_Bool
estate_machine_transition_add(Estate_Machine          *mach,
                              const Estate_Transition *tr);

EAPI Estate_State *estate_machine_current_state_get(const Estate_Machine *mach);

EAPI void estate_machine_lock(Estate_Machine *mach, const Estate_State *current);

/**
 * @}
 */

EAPI Eina_Mempool *estate_mempool_add(void);

#endif /* ! _ESTATE_H_ */

