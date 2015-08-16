#ifndef _ESTATE_PRIVATE_H_
#define _ESTATE_PRIVATE_H_

#include "Estate.h"

extern int _estate_log_dom;

#if 0
extern int ESTATE_EVENT_ENTERER;
extern int ESTATE_EVENT_EXITER;
extern int ESTATE_EVENT_TRANSITION;
#endif

typedef struct _State_Cb_Wrapper State_Cb_Wrapper;

struct _State_Cb_Wrapper
{
   EINA_INLIST;
   Estate_Cb       func;
   void           *data;
};

struct _Estate_Machine
{
   //Eina_Mempool         *mempool;

   /* Arrays */
   Estate_State         *states;
   unsigned int          states_count;
   unsigned int          st_curr;

   Estate_Transition    *transit;
   unsigned int          transit_count;
   unsigned int          tr_curr;

   Estate_State         *current_state;
   Eina_Hash            *data;
   //Ecore_Event_Handler  *evt[3];
};

struct _Estate_State
{
   const char       *name;
   State_Cb_Wrapper *cb[2];

   Estate_Transition  **transit;
   unsigned int         transit_count;
};

struct _Estate_Transition
{
   const char *name;

   Estate_State *from;
   Estate_State *to;

   struct {
      Estate_Cb   func;
      void       *data;
   } cb;
};

#define CRI(...) EINA_LOG_DOM_CRIT(_estate_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_estate_log_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_estate_log_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_estate_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_estate_log_dom, __VA_ARGS__)


#endif /* ! _ESTATE_PRIVATE_H_ */

