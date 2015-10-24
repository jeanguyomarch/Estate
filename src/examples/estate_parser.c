#include <Estate.h>
#include <Eina.h>
#include "estate_parser.est.c"

Estate_Machine *estate_fsm_estate_parser_load(void) EINA_MALLOC;

typedef struct _Parser Parser;
typedef struct _Fsm Fsm;
typedef struct _Transit Transit;
typedef struct _State State;
typedef struct _Cb Cb;
typedef struct _Fsm_Wrapper Fsm_Wrapper;

Parser *estate_cc_parser_new(void);
void estate_cc_parser_free(Parser *p);
Eina_Bool estate_cc_parser_file_set(Parser *p, char const *file);
void estate_cc_parser_file_unset(Parser *p);
Eina_List *estate_cc_parser_parse(Parser *p);
void estate_cc_parser_parse_free(Parser *p);

void estate_cc_data_print(Eina_List *fsm);

Eina_Bool
estate_cc_out_pgf_tikz(Eina_List  *parse,
                       const char *file);

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

enum
{
   COMMENT_NONE         = 0,
   COMMENT_SINGLE_LINE,
   COMMENT_MULTIPLE_LINES
};
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

   Transit **trs;
   unsigned int trs_count;
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



struct _Parser
{
   Eina_File  *ef;
   Eina_List  *parse;

   unsigned char *start;
   unsigned char *end;
   unsigned char *ptr;

   unsigned int line;
   unsigned int col;

   unsigned int anonymous;
   unsigned char comments;
   Eina_Bool stop_word;
   Eina_Bool has_token;
};

static char EINA_UNUSED
_char_next_get(Parser *p)
{
   char c;

   if (p->ptr > p->end) return EOF;

   c = *(p->ptr++);
   switch (c)
     {
      case '\0':
      case EOF:
         return EOF;

      case '\n':
         p->line++;
         p->col = 1;
         if (p->comments == COMMENT_SINGLE_LINE)
           p->comments = COMMENT_NONE;
         break;

      default:
         p->col++;
         break;
     }

   return c;
}


Estate_Machine *
estate_fsm_estate_parser_load(void)
{
   return _estate_fsm_estate_parser_load();
}

Parser *
estate_cc_parser_new(void)
{
   Parser *p;

   p = calloc(1, sizeof(*p));
   if (EINA_UNLIKELY(!p))
     {
        fprintf(stderr, "Failed to allocate Parser\n");
        return NULL;
     }
   return p;
}

void
estate_cc_parser_free(Parser *p)
{
   if (!p) return;
   estate_cc_parser_parse_free(p);
   estate_cc_parser_file_unset(p);
   free(p);
}

void
estate_cc_parser_file_unset(Parser *p)
{
   EINA_SAFETY_ON_NULL_RETURN(p);
   if (p->ef)
     {
        if (p->start) eina_file_map_free(p->ef, p->start);
        eina_file_close(p->ef);
        p->ef = NULL;
     }
}

void
estate_cc_parser_parse_free(Parser *p)
{
   EINA_SAFETY_ON_NULL_RETURN(p);

   Fsm *fsm;

   if (!p->parse) return;
   EINA_LIST_FREE(p->parse, fsm)
      fsm_free(fsm);
   p->parse = NULL;
}


Eina_Bool
estate_cc_parser_file_set(Parser     *p,
                          char const *file)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(p, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(file, EINA_FALSE);

   /* Open file with Eina_File */
   p->ef = eina_file_open(file, EINA_FALSE);
   if (EINA_UNLIKELY(!p->ef))
     {
        fprintf(stderr, "Failed to set parser with file \"%s\"\n", file);
        goto fail;
     }

   /* Map contents of the file */
   p->start = eina_file_map_all(p->ef, EINA_FILE_SEQUENTIAL);
   if (EINA_UNLIKELY(!p->start))
     {
        fprintf(stderr, "Failed to map sequentially contents of file \"%s\"\n", file);
        goto fail;
     }
   p->end = p->start + eina_file_size_get(p->ef);
   p->ptr = p->start;
   p->line = 1;
   p->col = 1;
   //p->sm = SM_NONE;
   p->has_token = EINA_FALSE;
   p->stop_word = EINA_FALSE;

   return EINA_TRUE;

fail:
   estate_cc_parser_file_unset(p);
   return EINA_FALSE;
}

State *
state_new(const char *name,
          const int   len)
{
   State *s;
   s = calloc(1, sizeof(*s));
   s->name = eina_stringshare_add_length(name, len);
   return s;
}

void
state_free(State *s)
{
   if (s->enterer.func) eina_stringshare_del(s->enterer.func);
   if (s->enterer.data) eina_stringshare_del(s->enterer.data);
   if (s->exiter.func) eina_stringshare_del(s->exiter.func);
   if (s->exiter.data) eina_stringshare_del(s->exiter.data);
   eina_stringshare_del(s->name);
   if (s->trs) free(s->trs);
   free(s);
}

Transit *
transit_new(const char *name,
            const int   len)
{
   Transit *t;
   t = calloc(1, sizeof(*t));
   t->name = eina_stringshare_add_length(name, len);
   return t;
}

void
transit_free(Transit *t)
{
   eina_stringshare_del(t->name);
   eina_stringshare_del(t->from);
   eina_stringshare_del(t->to);
   if (t->cb.func) eina_stringshare_del(t->cb.func);
   if (t->cb.data) eina_stringshare_del(t->cb.data);
   free(t);
}

Fsm *
fsm_new(const char *name,
        const int   len)
{
   /* TODO error cases */
   Fsm *f;
   f = calloc(1, sizeof(*f));
   f->name = eina_stringshare_add_length(name, len);
   f->transitions = eina_hash_stringshared_new(EINA_FREE_CB(transit_free));
   f->states = eina_hash_stringshared_new(EINA_FREE_CB(state_free));

   return f;
}

void
fsm_free(Fsm *f)
{
   if (!f) return;
   eina_stringshare_del(f->name);
   eina_hash_free(f->transitions);
   eina_hash_free(f->states);
   free(f);
}


int
main(int    argc,
     char **argv)
{
   Parser *p;
   Eina_Bool chk;
   Estate_Machine *fsm;
   //int c;
   //char buf[2048];
   //unsigned int k = 0;

   if (argc != 2) return 1;
   p = estate_cc_parser_new();
   chk = estate_cc_parser_file_set(p, argv[1]);
   if (!chk) return 1;

   fsm = estate_fsm_estate_parser_load();



   estate_machine_free(fsm);
   estate_cc_parser_free(p);
   return 0;
}

