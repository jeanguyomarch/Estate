#ifndef _ESTATE_PRIVATE_H_
#define _ESTATE_PRIVATE_H_

#include "Estate.h"

extern int _estate_log_dom;

#if 0
extern int ESTATE_EVENT_ENTERER;
extern int ESTATE_EVENT_EXITER;
extern int ESTATE_EVENT_TRANSITION;
#endif

typedef struct _Estate_Cb_Wrapper Estate_Cb_Wrapper;

struct _Estate_Cb_Wrapper
{
   Estate_Cb             func;
   void                 *data;
   Eina_Stringshare     *key;
};

struct _Estate_Machine
{
   Eina_Array           *states;
   Eina_Array           *transit;
   Estate_State         *current_state;
   Eina_Hash            *data;
};

struct _Estate_State
{
   Eina_Stringshare     *name;
   Estate_Cb_Wrapper     cb[2];
   Eina_Array           *transit;
};

struct _Estate_Transition
{
   Eina_Stringshare     *name;
   Estate_State         *from;
   Estate_State         *to;
   Estate_Machine       *mach;
   Estate_Cb_Wrapper     cb;
};

Eina_Bool _estate_misc_cb_cache(Estate_Machine *mach, Estate_Cb_Wrapper *wrp);

void _estate_state_cb_call(Estate_Machine *mach, Estate_State *st, const Estate_Transition *tr, Estate_Cb_Type type);

#define CRI(...) EINA_LOG_DOM_CRIT(_estate_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_estate_log_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_estate_log_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_estate_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_estate_log_dom, __VA_ARGS__)


#endif /* ! _ESTATE_PRIVATE_H_ */

