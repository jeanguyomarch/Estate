/**
 * @file Estate.h
 * @author Jean Guyomarc'h <jean.guyomarch@gmail.com>
 * @brief The Estate API documentation
 *
 * Estate is a library to create and manipulate finite
 * state machines (FSM).
 *
 * @mainpage Estate
 * @date 2015 (created)
 *
 * @section toc Table of Contents
 *
 * @li @ref estate_intro
 * @li @ref estate_compiling
 * @li @ref estate_syntax
 * @li @ref estate_compiler
 * @li @ref estate_api
 *
 * @section estate_intro Introduction
 *
 * Estate is both a C library and a descriptive language to create and
 * manipulate finite state machines (FSM).
 *
 * A FSM can be described by its transitions only, a transition
 * being a unidirectional relation from a state to another.
 * However, this is of no use if one cannot attach event handlers
 * to trigger events when a state changes (a transition occurs).
 *
 * Estate allows you to attach a callback to each transition, which will
 * be called when the transition will be activated.
 * Furthermore, each state can register two callbacks:
 * @li the enterer callback: called when a transition points to the state
 * @li the exiter callback: called when a transition leaves the state
 *
 * Furthermore, you can provide a user data to each callback. User data
 * must be uniquely identified by a key (a string), so the program
 * can provide it later.
 *
 *
 * This is how you should use Esate:
 * @li describe your finite state machine in an Estate (.est) file
 * @li generate a C boilerplate with `estate_cc`: `estate_cc --gi file.est.c -o file.c file.est`
 * @li generate the C implementation of your FSM, which uses the Estate library,
 *     with `estate_cc`: `estate_cc --gc -o file.est.c file.est`
 * @li in your boilerplate, implement your callbacks
 * @li in your boilerplate, attach user data if needed
 * @li that's it!!
 *
 *
 * @section estate_syntax Estate Descriptive Syntax
 *
 * The estate language is very easy to understand and write.
 * The reference of the language is defined here: @ref Estate_Ref
 *
 *
 * @section estate_compiler The Estate Compiler
 * @todo
 *
 *
 * @section estate_compiling How to compile
 *
 * You must link your program to the Estate library. Estate supports
 * pkg-config which makes the task easier:
 *
   @verbatim
   gcc -Wall -Wextra prog.c $(pkg-config --cflags --libs estate)
   @endverbatim
 *
 * But you may also want to take advantage of the Estate descriptive
 * language, write your FSM in a dedicated file, describe it in a
 * comprehensive manner, and generate automatically an Estate FSM.
 *
 *
 *
 * @section estate_api The Estate Application Programming Interface (API)
 *
 * The API is split into several modules:
 * @li @ref Estate_Main
 * @li @ref Estate_Machine
 * @li @ref Estate_State
 * @li @ref Estate_Transition
 *
 * Estate developers can access the internal API documentation:
 * @li @ref Estate_Private
 */

#ifndef _ESTATE_H_
#define _ESTATE_H_

#include <Eina.h>

/**
 * @defgroup Estate_Main Top level functions
 * API that affect Estate as a whole
 */

/**
 * @defgroup Estate_State State API
 * API to manipulate States in an FSM
 */

/**
 * @defgroup Estate_Transition Transition API
 * API to manipulate Transitions in an FSM
 */

/**
 * @defgroup Estate_Machine Finite State Machine API
 * API to manipulate finite state machines objects
 */




/**
 * @addtogroup Estate_Machine
 * @{
 * @typedef Estate_Machine
 * An opaque handler for a finite state machine
 */
typedef struct _Estate_Machine Estate_Machine;
/** @} */

/**
 * @addtogroup Estate_State
 * @{
 * @typedef Estate_State
 * An opaque handler for a state
 */
typedef struct _Estate_State Estate_State;
/** @} */

/**
 * @addtogroup Estate_Transition
 * @{
 * @typedef Estate_Transition
 * An opaque handler for a transition
 */
typedef struct _Estate_Transition Estate_Transition;
/** @} */

/**
 * @addtogroup Estate_Main
 * @{
 */

/**
 * @enum Estate_Cb_Type
 * Type of an Estate callback
 */
typedef enum
{
   ESTATE_CB_TYPE_ENTERER         = 0, /**< When entering a state */
   ESTATE_CB_TYPE_EXITER          = 1, /**< When exiting a state */
   ESTATE_CB_TYPE_TRANSITION      = 2  /**< When a transition is executing */
} Estate_Cb_Type;

/**
 * @typedef Estate_Cb
 * Callback raised when a transition is activated, and therefore
 * the current state is changing
 *
 * @param data User data. Provided by estate_machine_data_set()
 * @param type Which callback has been triggered
 * @param transition The transition related to the callback
 */
typedef int (*Estate_Cb)(void                    *data,
                         Estate_Cb_Type           type,
                         const Estate_Transition *transition);

/**
 * @typedef Estate_Cb_Ctor
 * Type to provide to init functions callback parameters
 */
typedef struct _Estate_Cb_Ctor Estate_Cb_Ctor;

/**
 * @struct _Estate_Cb_Ctor
 * Contains the information for a better initialization of
 * callbacss
 */
struct _Estate_Cb_Ctor
{
   Estate_Cb     func;
   const char   *key;
   unsigned int  key_len;
};


/**
 * @def ESTATE_CB_OK
 * The default return value of the estate callbacks. It
 * means no error were encountered during the execution
 * of the callback.
 */
#define ESTATE_CB_OK 0

/**
 * Initialises the Estate library
 * @return How many times the library has been initialized.
 */
EAPI int estate_init(void);

/**
 * Shuts the Estate library down
 * @return How many times the library has been initialized.
 */
EAPI int estate_shutdown(void);

/**
 * @}
 */



/**
 * @addtogroup Estate_State
 * @{
 */

/**
 * Allocates a state in a given finite state machine
 *
 * @param mach The parent finite state machine
 * @param transit_count The number of transitions outgoing this state
 * @return The allocated state. NULL on failure
 */
EAPI Estate_State *estate_state_new(Estate_Machine *mach,
                                    unsigned int    transit_count);

/**
 * Releases a state allocated by estate_state_new()
 *
 * @param st The state to be released. Shall not be NULL.
 */
EAPI void estate_state_free(Estate_State *st);

/**
 * Sorting function that is aimed to be used by qsort() and friends.
 * It must be used to sort an array of transitions before passing it
 * to estate_state_init()
 *
 * @param d1 First transition to compare
 * @param d2 Second transition to compare
 * @return see the qsort() manual
 */
EAPI int estate_state_sort_transitions_cb(const void *d1,
                                          const void *d2);

/**
 * Sets a pre-allocated state up
 *
 * @param st The pre-allocated state to set up
 * @param name The name to attach to the state
 * @param transitions An array of the @c transit_count transitions outgoing
 *        from the state @c st. It must be sort with estate_state_sort_transitions_cb()
 *        before being passed to this function
 * @param transit_count The count of transitions in the @c transitions array
 * @param enterer The function to be called when a transition ends to the state @c st
 * @param enterer_datakey The string used to retrieve the user data upon the first 
 *        call to @c enterer
 * @param exiter The function to be called when a transition leaves from the state @c st
 * @param exiter_datakey The string used to retrieve the user data upon the first
 *        call to @c exiter
 * @return EINA_TRUE on success, EINA_FALSE on failure
 *
 * @see estate_state_sort_transitions_cb()
 */
EAPI Eina_Bool
estate_state_init(Estate_State             *st,
                  const char               *name,
                  const Estate_Transition **transitions,
                  unsigned int              transit_count,
                  const Estate_Cb_Ctor     *enterer,
                  const Estate_Cb_Ctor     *exiter);

/**
 * Releases the internals of a state
 *
 * @param st The state to release
 */
EAPI void estate_state_deinit(Estate_State *st);

/**
 * Returns the name of the provided state
 *
 * @param st The state which name must be retrieved
 * @return The name of the state. NULL on failure.
 */
EAPI const char *estate_state_name_get(const Estate_State *st);

/**
 * @}
 */


/**
 * @addtogroup Estate_Transition
 * @{
 */

/**
 * Allocates a transition in a given finite state machine
 *
 * @param mach The parent finite state machine
 * @return The allocated state. NULL on failure.
 */
EAPI Estate_Transition *estate_transition_new(Estate_Machine *mach);

/**
 * Releases a state allocated bu estate_transition_new()
 *
 * @param tr The transition to be released. Shall not be NULL
 */
EAPI void estate_transition_free(Estate_Transition *tr);

/**
 * Sets a pre-allocated transition up
 *
 * @param tr The pre-allocated transition to set up
 * @param name The name to attach to the transition
 * @param from The state from which @c tr starts
 * @param to The state from which @c tr ends
 * @param cb Helper to set the transition callback. Must be filled manually
 *           before being passed (e.g. on the stack)
 * @param st_enterer Helper to set the callback called when the transition
 *                   enters the @c to state. Must be filled manually
 *                   before being passed (e.g. on the stack)
 * @param st_exiter Helper to set the callback called when the transition
 *                  exits the @c from state. Must be filled manually
 *                  before being passed (e.g. on the stack)
 * @return EINA_TRUE on success, EINA_FALSE on failure
 */
EAPI Eina_Bool
estate_transition_init(Estate_Transition    *tr,
                       const char           *name,
                       const Estate_State   *from,
                       const Estate_State   *to,
                       const Estate_Cb_Ctor *cb,
                       const Estate_Cb_Ctor *st_enterer,
                       const Estate_Cb_Ctor *st_exiter);

/**
 * Releases the internals of a transition
 *
 * @param tr The transition to release
 */
EAPI void estate_transition_deinit(Estate_Transition *tr);

/**
 * Returns the name of the provided transition
 *
 * @param tr The transition which name must be retrieved
 * @return The name of the transition. NULL on failure.
 */
EAPI const char *estate_transition_name_get(const Estate_Transition *tr);

/**
 * @}
 */


/**
 * @addtogroup Estate_Machine
 * @{
 */

/**
 * Creates a finite state machine (FSM) handler.
 * When this function returns, the FSM is considered UNLOCKED:
 * it is possible to edit it (i.e. construct) until
 * estate_machine_lock() is called.
 *
 * @param states The count of states in the finite state machine
 * @param transitions The count of transitions in the finite state machine
 * @return The finite state machine handler. NULL on failure
 */
EAPI Estate_Machine *
estate_machine_new(unsigned int states,
                   unsigned int transitions);

/**
 * Releases a finite state machine handler
 *
 * @param mach The finite state machine handler to be released
 *
 * @see estate_machine_new()
 */
EAPI void estate_machine_free(Estate_Machine *mach);

EAPI int estate_machine_cb_check(const Estate_Machine *mach,
                                 Estate_Cb_Type        type);

/**
 * Associates a key to a value to be retrieved when a callback is called.
 * Fails if the FSM is locked.
 *
 * @param mach The FSM handler
 * @param key The key used to store and retrieve persistent user data @c value
 * @param value The value retrieved thanks to the key @c key
 * @return EINA_TRUE on success, EINA_FALSE on failure
 *
 * @see estate_machine_lock()
 */
EAPI Eina_Bool
estate_machine_data_set(Estate_Machine *mach,
                        const char     *key,
                        const void     *value);

/**
 * Attach a user pointer to the provided FSM.
 * It will be available from every states and transitions
 * through the Estate_Machine handler.
 *
 * @param mach The FSM which global data must be set
 * @param data The data to be set. It's lifetime is not managed by @c mach
 * @see estate_machine_global_data_get()
 */
EAPI void
estate_machine_global_data_set(Estate_Machine *mach,
                               const void     *data);

/**
 * Retrieves the global data attached to a FSM
 * @param mach The finite state machine to query
 * @return The global data attached to @c mach
 * @see estate_machine_global_data_set()
 */
EAPI void *
estate_machine_global_data_get(const Estate_Machine *mach);

/**
 * Registers a new state in the finite state machine.
 * Fails if the FSM is locked.
 *
 * @param mach The FSM handler
 * @param state The state to register
 * @return EINA_TRUE on success, EINA_FALSE on failure
 *
 * @see estate_machine_lock()
 */
EAPI Eina_Bool
estate_machine_state_add(Estate_Machine     *mach,
                         const Estate_State *state);

/**
 * Registers a new transition in the finite state machine
 * Fails if the FSM is locked.
 *
 * @param mach The FSM handler
 * @param tr The transition to register
 * @return EINA_TRUE on success, EINA_FALSE on failure
 * @see estate_machine_lock()
 */
EAPI Eina_Bool
estate_machine_transition_add(Estate_Machine          *mach,
                              const Estate_Transition *tr);

/**
 * Gets the current state in the finite state machine
 * Fails if the FSM is NOT locked.
 *
 * @param mach The FSM handler
 * @return The current state in the finite state machine
 * @see estate_machine_lock()
 */
EAPI Estate_State *estate_machine_current_state_get(const Estate_Machine *mach);

/**
 * Locks the finite state machine handler. After this function returns, the FSM
 * cannot be edited (transitions and states cannot be added, data keys cannot
 * be modified) and estate_machine_current_state_get() becomes valid and
 * will return @c current until a transition occurs.
 *
 * @param mach The FSM handler
 * @param current The init state: starting point of the FSM
 */
EAPI void estate_machine_lock(Estate_Machine *mach, const Estate_State *current);

/**
 * If the current state can make the transition @c transition, then the FSM
 * does the transition:
 *   - calls the exiter callback (if any) from the current state
 *   - calls the transition callback (if any) for the current transition
 *   - calls the enterer callback (if any) (from the next state)
 * Fails if the FSM is NOT locked.
 *
 * @param mach The FSM handler
 * @param transition The transition that the current state should do
 * @return EINA_TRUE on success, EINA_FALSE on failure
 * @see estate_machine_lock()
 */
EAPI Eina_Bool
estate_machine_transition_do(Estate_Machine *mach,
                             const char     *transition);

/**
 * @}
 */

EAPI Eina_Mempool *estate_mempool_add(void);

#endif /* ! _ESTATE_H_ */

