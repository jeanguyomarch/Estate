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
   void          *base; /**< Address returned by the default allocator */
   unsigned char *ptr;  /**< Allocator's internal stack pointer */
   size_t         mem;  /**< Total memory allocated */
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
};

/**
 * @struct _Estate_Machine
 * Internals of an Estate finite state machine
 */
struct _Estate_Machine
{
   Estate_Mempool       *mempool; /**< Allocator of the whole FSM */

   Eina_Array           *states; /**< Array of all states */
   Eina_Array           *transit; /**< Array of all transitions */
   Estate_State         *current_state; /**< Points on the current state */

   Eina_Hash            *data; /**< Hash that contains all user data. Searchable
                                 by Eina_Stringshare keys */

   Eina_Bool             locked; /**< EINA_FALSE when the FSM is being built,
                                   EINA_TRUE when ready */
};

/**
 * @struct _Estate_State
 * Internals of an Estate state
 */
struct _Estate_State
{
   Eina_Stringshare     *name;    /**< Name of the state */
   Estate_Cb_Wrapper     cb[2];   /**< Enterer and Exiter callbacks */
   Eina_Array           *transit; /**< Array of transitions pointers outgoing
                                    from this state */
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
 * Wraps the call to a callback by checking if the callback is set,
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
 * @}
 */

#endif /* ! _ESTATE_PRIVATE_H_ */

