#ifndef _ESTATE_CC_H_
#define _ESTATE_CC_H_

#include <Eina.h>

typedef struct _Parser Parser;
typedef struct _Fsm Fsm;
typedef struct _Transit Transit;
typedef struct _State State;

typedef enum
{
   PARSER_CODE_OK = 0
} Parser_Code;

struct _State
{
   Eina_Stringshare *name;
   struct _cb {
      Eina_Stringshare *func;
      Eina_Stringshare *data;
   } enterer, exiter, transition;
};

struct _Transit
{
   Eina_Stringshare *name;
   Eina_Stringshare *from;
   Eina_Stringshare *to;
};

struct _Fsm
{
   Eina_Stringshare *name;
   Eina_Array *transitions;
   Eina_Array *states;
};


extern int _estate_cc_dom;

Eina_Bool estate_cc_log_init(void);
void estate_cc_log_shutdown(void);

Parser *estate_cc_parser_new(void);
void estate_cc_parser_free(Parser *p);
Eina_Bool estate_cc_parser_file_set(Parser *p, char const *file);
void estate_cc_parser_file_unset(Parser *p);
Eina_List *estate_cc_parser_parse(Parser *p);
void estate_cc_parser_parse_free(Parser *p);

void estate_cc_data_print(Eina_List *fsm);

#define CRI(...) EINA_LOG_DOM_CRIT(_estate_cc_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_estate_cc_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_estate_cc_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_estate_cc_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_estate_cc_dom, __VA_ARGS__)

#endif /* ! _ESTATE_CC_H_ */

