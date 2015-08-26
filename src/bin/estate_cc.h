#ifndef _ESTATE_CC_H_
#define _ESTATE_CC_H_

#include <Eina.h>

typedef struct _Parser Parser;
typedef struct _Fsm Fsm;
typedef struct _Transit Transit;
typedef struct _State State;
typedef struct _Cb Cb;
typedef struct _Fsm_Wrapper Fsm_Wrapper;

typedef enum
{
   PARSER_CODE_OK = 0
} Parser_Code;

struct _Cb
{
   Eina_Stringshare *func;
   Eina_Stringshare *data;
};

struct _State
{
   Eina_Stringshare *name;
   Cb enterer;
   Cb exiter;

   /* For model checking */
   struct {
      unsigned int from;
      unsigned int to;
   } check;
};

struct _Transit
{
   Eina_Stringshare *name;
   Eina_Stringshare *from;
   Eina_Stringshare *to;

   Cb cb;
};

struct _Fsm
{
   Eina_Stringshare *name;
   Eina_Hash *transitions;
   Eina_Hash *states;
   Eina_Stringshare *init;
};

struct _Fsm_Wrapper
{
   Fsm   *fsm;
   FILE  *f;
   State *cstate;
};

extern int _estate_cc_dom;
unsigned int estate_cc_verbosity_get(void);

Eina_Bool estate_cc_log_init(void);
void estate_cc_log_shutdown(void);

Parser *estate_cc_parser_new(void);
void estate_cc_parser_free(Parser *p);
Eina_Bool estate_cc_parser_file_set(Parser *p, char const *file);
void estate_cc_parser_file_unset(Parser *p);
Eina_List *estate_cc_parser_parse(Parser *p);
void estate_cc_parser_parse_free(Parser *p);

void estate_cc_data_print(Eina_List *fsm);

Eina_Bool estate_cc_check_states(Eina_List *parse);

Eina_Bool
estate_cc_out_gi(Eina_List  *parse,
                 const char *file,
                 const char *include);

Eina_Bool
estate_cc_out_gc(Eina_List  *parse,
                 const char *file);

State *state_new(const char *name, const int len);
void state_free(State *s);
Transit *transit_new(const char *name, const int len);
void transit_free(Transit *t);
Fsm *fsm_new(const char *name, const int len);
void fsm_free(Fsm *f);


#define CRI(...) EINA_LOG_DOM_CRIT(_estate_cc_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_estate_cc_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_estate_cc_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_estate_cc_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_estate_cc_dom, __VA_ARGS__)

#define V(thr_) estate_cc_verbosity_get() >= (thr_)

#endif /* ! _ESTATE_CC_H_ */

