/**
 * @file estate_private.h
 * @author Jean Guyomarc'h <jean.guyomarch@gmail.com>
 * @brief The Estate internal API documentation
 *
 * Estate is a library to create and manipulate finite state
 * machines (FSM). This is its internal API.
 */

#ifndef _ESTATE_PRIVATE_H_
#define _ESTATE_PRIVATE_H_

#include "Estate.h"

/**
 * @defgroup Estate_Private Estate Private API
 * Private (internal) API of Estate
 * @{
 */

/**
 * Log domain used by Eina_Log. Must be extern because the log domain
 * has to be known outside of more than one compiling unit.
 */
extern int _estate_log_dom;

/**
 * @typedef Estate_Mempool
 * Handler to manipulate Estate's internal allocator
 */
typedef struct _Estate_Mempool Estate_Mempool;

/**
 * @typedef Estate_Cb_Wrapper
 * Hanlder to manipulate an Estate callback
 */
typedef struct _Estate_Cb_Wrapper Estate_Cb_Wrapper;


/**
 * @struct _Estate_Mempool
 * Estate's custom allocator
 */
struct _Estate_Mempool
{
   unsigned char *base; /**< Address returned by the default allocator */
   unsigned char *stack_states;
   unsigned char *stack_transits;
   unsigned char *end;  /**< Pointer to the end of the memory zone */
   unsigned int   states_reg;
   unsigned int   transit_reg;
};

/**
 * @struct _Estate_Cb_Wrapper
 * Estate's callback handler
 */
struct _Estate_Cb_Wrapper
{
   Estate_Cb             func; /**< Actual callback */
   void                 *data; /**< Cache where the userdata to be provided to
                                 @c func is stored */
   Eina_Stringshare     *key;  /**< String used to retrieve the user data */

   int                   result; /**< Result of the callback. 0 means no
                                   error */
};

/**
 * @struct _Estate_Machine
 * Internals of an Estate finite state machine
 */
struct _Estate_Machine
{
   Estate_Mempool      *mempool; /**< Allocator of the whole FSM */

   Estate_State        *states; /**< Array of all states */
   Estate_Transition   *transit; /**< Array of all transitions */
   Estate_State        *current_state; /**< Points to the current state */
   Estate_Transition   *current_transition; /**< Points to the last executed
                                              transition */

   Estate_Error_Cb      error_cb; /** Function called on error */

   Eina_Hash           *data; /**< Hash that contains all user data. Searchable
                                 by Eina_Stringshare keys */

   Eina_List           *cb_defer; /**< List of callbacks to be deferred */

   void                *global_data; /**< Global data, accessible by all states and transitions */

   unsigned int states_count;
   unsigned int transit_count;


   unsigned char        in_cb; /**< Used to prevent making a transition in a
                                 callback but defer then if in an entering cb */
   Eina_Bool            locked; /**< EINA_FALSE when the FSM is being built,
                                   EINA_TRUE when ready */
};

/**
 * @struct _Estate_State
 * Internals of an Estate state
 */
struct _Estate_State
{
   Eina_Stringshare     *name;    /**< Name of the state */
   Estate_Cb_Wrapper     enterer; /**< Enterer callback */
   Estate_Cb_Wrapper     exiter;  /**< Exiter callback */
   Estate_Transition   **transit; /**< Array of transitions pointers outgoing
                                    from this state */
   unsigned int          transit_count; /**< Count of transitions */
};

/**
 * @struct _Estate_Transition
 * Internals of an Estate transition
 */
struct _Estate_Transition
{
   Eina_Stringshare     *name; /**< Name of the transition */
   Estate_State         *from; /**< State from which originates the transition */
   Estate_State         *to;   /**< Destination state */
   Estate_Machine       *mach; /**< Points to its finite state machine */
   Estate_Cb_Wrapper     cb;   /**< Callback to be executed when the transition
                                 is being executed */

   Estate_Cb_Wrapper     st_enterer; /**< May be used when this transition enters
                                       its @c to state */
   Estate_Cb_Wrapper     st_exiter; /**< May be used when this transition exits
                                      its @c from state */
};

/**
 * Caches the user data in a callback structure.
 * If the data is already in cache, returns immediately.
 * If there is no data to retrieve, returns immediately.
 *
 * @param mach The state machine handler
 * @param wrp Callback wrapper whose user data must be cached
 * @return EINA_TRUE on success, EINA_FALSE on failure
 */
Eina_Bool _estate_misc_cb_cache(Estate_Machine *mach, Estate_Cb_Wrapper *wrp);

/**
 * Wraps the call to a state callback by checking if the callback is set,
 * does the caching operation and calls the callback
 *
 * @param mach The state machine handler
 * @param st The state which originates a call
 * @param tr The transition which triggererd the state callback
 * @param type The type of the callback (ESTATE_CB_TYPE_ENTERER or ESTATE_CB_TYPE_EXITER)
 * @see _estate_misc_cb_cache()
 */
void _estate_state_cb_call(Estate_Machine *mach, Estate_State *st, const Estate_Transition *tr, Estate_Cb_Type type);

/**
 * Wraps the call to a callback by checking if the callback is set,
 * does the caching operation and calls the callback
 *
 * @param mach The state machine handler
 * @param wrp The callback wrapper
 * @param type The type of the callback (ESTATE_CB_TYPE_ENTERER or ESTATE_CB_TYPE_EXITER)
 * @param tr The transition which triggererd the state callback
 * @see _estate_misc_state_cb_call()
 */
void
_estate_misc_cb_call(Estate_Machine          *mach,
                     Estate_Cb_Wrapper       *wrp,
                     Estate_Cb_Type           type,
                     const Estate_Transition *tr);

/**
 * @def CRI(...)
 * Logs in the estate domain with a critical level
 */
#define CRI(...) EINA_LOG_DOM_CRIT(_estate_log_dom, __VA_ARGS__)

/**
 * @def ERR(...)
 * Logs in the estate domain with an error level
 */
#define ERR(...) EINA_LOG_DOM_ERR(_estate_log_dom, __VA_ARGS__)

/**
 * @def WRN(...)
 * Logs in the estate domain with a warning level
 */
#define WRN(...) EINA_LOG_DOM_WARN(_estate_log_dom, __VA_ARGS__)

/**
 * @def INF(...)
 * Logs in the estate domain with an informational level
 */
#define INF(...) EINA_LOG_DOM_INFO(_estate_log_dom, __VA_ARGS__)

/**
 * @def DBG(...)
 * Logs in the estate domain with a debug level
 */
#define DBG(...) EINA_LOG_DOM_DBG(_estate_log_dom, __VA_ARGS__)

/**
 * @def CB_UNLOCKED
 * Value used to declare no callback are running
 */
#define CB_UNLOCKED 0xff

/**
 * @}
 */



Estate_Mempool *
_estate_mempool_new(unsigned int states,
                    unsigned int transitions,
                    Eina_Bool    zero);

void
_estate_mempool_free(Estate_Mempool *mp);

Estate_State *
_estate_mempool_state_push(Estate_Mempool *mp);

Estate_Transition *
_estate_mempool_transition_push(Estate_Mempool *mp);

#endif /* ! _ESTATE_PRIVATE_H_ */

