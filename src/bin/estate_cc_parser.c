#include "estate_cc.h"


enum
{
   COMMENT_NONE         = 0,
   COMMENT_SINGLE_LINE,
   COMMENT_MULTIPLE_LINES
};

typedef enum
{
   SM_NONE = 0,
   SM_FSM,
   SM_TRANSITIONS,
   SM_TRANSITION_START,
   SM_TRANSITION_FROM,
   SM_TRANSITION_TO,
   SM_TRANSITION,
   SM_TRANSITION_CB,
   SM_TRANSITION_CB_FUNC_PROP,
   SM_TRANSITION_CB_DATA_PROP,
   SM_STATES,
   SM_STATE,
   SM_STATE_EOB,
   SM_STATE_CB,
   SM_STATE_CB_FUNC_PROP,
   SM_STATE_CB_DATA_PROP,
   SM_STATE_PROPERTY
} Sm;

struct _Parser
{
   Eina_File  *ef;
   Eina_List  *parse;

   unsigned char *start;
   unsigned char *end;
   unsigned char *ptr;

   unsigned int line;
   unsigned int col;

   unsigned char comments;
   unsigned char sm;
   Eina_Bool stop_word;
   Eina_Bool has_token;
};


static char
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

static Eina_Bool
_sm_block_is(const Parser *p)
{
   switch (p->sm)
     {
      case SM_NONE:
      case SM_FSM:
      case SM_TRANSITIONS:
      case SM_TRANSITION_START:
      case SM_TRANSITION:
      case SM_STATE:
      case SM_STATES:
      case SM_STATE_CB:
      case SM_STATE_PROPERTY:
         return EINA_TRUE;

      default:
         return EINA_FALSE;
     }
}

Parser *
estate_cc_parser_new(void)
{
   Parser *p;

   p = calloc(1, sizeof(*p));
   if (EINA_UNLIKELY(!p))
     {
        CRI("Failed to allocate Parser");
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
        ERR("Failed to set parser with file \"%s\"", file);
        goto fail;
     }

   /* Map contents of the file */
   p->start = eina_file_map_all(p->ef, EINA_FILE_SEQUENTIAL);
   if (EINA_UNLIKELY(!p->start))
     {
        CRI("Failed to map sequentially contents of file \"%s\"", file);
        goto fail;
     }
   p->end = p->start + eina_file_size_get(p->ef);
   p->ptr = p->start;
   p->line = 1;
   p->col = 1;
   p->sm = SM_NONE;
   p->has_token = EINA_FALSE;
   p->stop_word = EINA_FALSE;

   return EINA_TRUE;

fail:
   estate_cc_parser_file_unset(p);
   return EINA_FALSE;
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

Eina_List *
estate_cc_parser_parse(Parser *p)
{
   char buf[2048];
   char c;
   unsigned int k = 0;
   Fsm *f = NULL;
   Transit *t = NULL;
   State *s = NULL;
   Cb *cb = NULL;
   Eina_Stringshare *sh;

#define PARSE_ERROR(fmt_, ...) \
   do { \
      ERR("Parse Error (%u,%u): "fmt_, p->line, p->col - 1, ## __VA_ARGS__); \
      goto fail; \
   } while (0)

   /* Parse the file character by character, in one go */
   for (c = _char_next_get(p); c != EOF; c = _char_next_get(p))
     {
        /* Comments... skip */
        if (p->comments != COMMENT_NONE)
          continue;

        /* Check for characters with a special meaning... */
        switch (c)
          {
             /* Check for comments */
           case '/':
              c = _char_next_get(p);
              if (c == '/')
                p->comments = COMMENT_SINGLE_LINE;
              else if (c == '*')
                p->comments = COMMENT_MULTIPLE_LINES;
              else
                {
                   PARSE_ERROR("Invalid character");
                   goto fail;
                }
              break;

              /* Start of block */
           case '{':
              if (_sm_block_is(p))
                p->has_token = EINA_TRUE;
              else
                PARSE_ERROR("Invalid '{'. Sm is %i", p->sm);
              break;

              /* Start of state property */
           case '@':
              if (p->sm == SM_STATES)
                p->has_token = EINA_TRUE;
              else
                PARSE_ERROR("Unexpected '@'. SM = %i", p->sm);
              break;

              /* End of property name */
           case ':':
              if ((p->sm == SM_STATE_CB) ||
                  (p->sm == SM_TRANSITION_START))
                p->has_token = EINA_TRUE;
              else
                PARSE_ERROR("Invalid ':' current state is %i", p->sm);
              break;

              /* End of property */
           case ';':
              if ((p->sm == SM_STATE_CB_FUNC_PROP) ||
                  (p->sm == SM_STATE_CB_DATA_PROP) ||
                  (p->sm == SM_TRANSITION_CB_FUNC_PROP) ||
                  (p->sm == SM_TRANSITION_CB_DATA_PROP) ||
                  (p->sm == SM_TRANSITION_TO))
                p->has_token = EINA_TRUE;
              else
                PARSE_ERROR("Invalid ';'. SM %i", p->sm);
              break;

              /* Transition (from) -> (to) */
           case '-':
              if (p->sm != SM_TRANSITION_FROM)
                PARSE_ERROR("Unexcepted character '-': not in a transition block."
                            " SM: %i", p->sm);
              c = _char_next_get(p);
              if (c == '>')
                p->has_token = EINA_TRUE;
              else
                PARSE_ERROR("Unexcepted character '%c'", c);
              break;

              /* End of block. case ENDING_BLOCK: p->sm = PARENT_BLOCK; */
           case '}':
              switch (p->sm)
                {
                 case SM_STATE_CB:
                    p->sm = SM_STATE;
                    break;

                 case SM_STATE:
                    p->sm = SM_STATES;
                    break;

                 case SM_STATES:
                    p->sm = SM_FSM;
                    break;

                 case SM_TRANSITIONS:
                    p->sm = SM_FSM;
                    break;

                 case SM_TRANSITION_START:
                    p->sm = SM_TRANSITIONS;
                    break;

                 case SM_TRANSITION_CB:
                    p->sm = SM_TRANSITION;
                    break;

                 case SM_TRANSITION:
                    p->sm = SM_TRANSITIONS;
                    break;

                 case SM_FSM:
                    p->sm = SM_NONE;
                    break;

                 default:
                    PARSE_ERROR( "Unexpected end of block '}'");
                }
              break;

              /* Whitespaces */
           case ' ':
           case '\n':
           case '\r':
           case '\t':
              /* Whitespace in the middle of a word must not be skipped! */
              if (k != 0)
                p->stop_word = EINA_TRUE;
              break;

              /* Read a token - cannot start with a digit */
           case '0' ... '9':
              /* Don't start with a digit (because C does it...) */
              if (k == 0)
                PARSE_ERROR("Token cannot start with a digit");
           case '_':
           case 'a' ... 'z':
           case 'A' ... 'Z':
              /* To enforce no whitespaces between words */
              if (p->stop_word)
                PARSE_ERROR("Invalid start of token");
              /* Register the character as being part of the next token */
              buf[k++] = c;
              break;

           default:
              PARSE_ERROR("Invalid character '%c'", c);
          }


        /* Parsing: a token was found during the previous passes */
        if (p->has_token)
          {
             /* I want a NUL-terminates string */
             buf[k] = 0;

             /* State machine: what to do with the token in function of what is
              * the internal state of the parser */
             switch (p->sm)
               {
                  /* Out of all blocks: create a new machine */
                case SM_NONE:
                   f = fsm_new(buf, k);
                   p->parse = eina_list_append(p->parse, f);
                   p->sm = SM_FSM; /* Start FSM */
                   break;

                   /* In the FSM block I can describe:
                    *   - transitions
                    *   - states
                    */
                case SM_FSM:
                   if (!strcmp(buf, "transitions")) /* Block transitions */
                     p->sm = SM_TRANSITIONS;
                   else if (!strcmp(buf, "states")) /* Block states */
                     p->sm = SM_STATES;
                   else
                     PARSE_ERROR("Unexpected token [%s]. SM is %i", buf, p->sm);
                   break;

                   /* Just found a new state description... */
                case SM_STATES:
                   s = state_new(buf, k);
                   if (eina_hash_find(f->states, s->name))
                     PARSE_ERROR("State [%s] has already been defined", s->name);
                   eina_hash_add(f->states, s->name, s);

                   /* If there is a property, start parse it.
                    * Otherwise, parse the state */
                   if (c == '@')
                     p->sm = SM_STATE_PROPERTY;
                   else
                     p->sm = SM_STATE;
                   break;

                   /* Internals of the state:
                    *   - enterer
                    *   - exiter
                    */
                case SM_STATE:
                   /* cb will points on the callback handler to fill on next state */
                   if (!strcmp(buf, "enterer"))
                     cb = &(s->enterer);
                   else if (!strcmp(buf, "exiter"))
                     cb = &(s->exiter);
                   else
                     PARSE_ERROR("Invalid token [%s]", buf);
                   p->sm = SM_STATE_CB;
                   break;

                case SM_STATE_PROPERTY:
                   if (!strcmp(buf, "init"))
                     {
                        if (f->init)
                          PARSE_ERROR("Duplicated property @init");
                        else
                          {
                             f->init = eina_stringshare_add(s->name);
                             p->sm = SM_STATE;
                          }
                     }
                   else
                     PARSE_ERROR("Invalid property [@%s]", buf);
                   break;

                   /* Internals of a state callback:
                    *   - func:
                    *   - data:
                    */
                case SM_STATE_CB:
                   if (!strcmp(buf, "func"))
                     {
                        if (cb->func)
                          PARSE_ERROR("Func already specified");
                        p->sm = SM_STATE_CB_FUNC_PROP;
                     }
                   else if (!strcmp(buf, "data"))
                     {
                        if (cb->data)
                          PARSE_ERROR("Data already specified");
                        p->sm = SM_STATE_CB_DATA_PROP;
                     }
                   else
                     PARSE_ERROR("Invalid token [%s]", buf);
                   break;

                   /* Register what is in func: */
                case SM_STATE_CB_FUNC_PROP:
                   cb->func = eina_stringshare_add_length(buf, k);
                   p->sm = SM_STATE_CB;
                   break;

                   /* Register what is in data: */
                case SM_STATE_CB_DATA_PROP:
                   cb->data = eina_stringshare_add_length(buf, k);
                   p->sm = SM_STATE_CB;
                   break;

                   /* Parse the transitions block */
                case SM_TRANSITIONS:
                   if (!strcmp(buf, "model"))
                     p->sm = SM_TRANSITION_START;
                   else
                     {
                        sh = eina_stringshare_add_length(buf, k);
                        t = eina_hash_find(f->transitions, sh);
                        if (!t)
                          PARSE_ERROR("Transition [%s] has not been previously defined", sh);
                        eina_stringshare_del(sh);
                        p->sm = SM_TRANSITION;
                     }
                   break;

                case SM_TRANSITION:
                   if (!strcmp(buf, "cb"))
                     p->sm = SM_TRANSITION_CB;
                   else
                     PARSE_ERROR("Invalid token [%s]", buf);
                   break;

                case SM_TRANSITION_CB:
                   if (!strcmp(buf, "func"))
                     {
                        if (t->cb.func)
                          PARSE_ERROR("Func already specified");
                        p->sm = SM_TRANSITION_CB_FUNC_PROP;
                     }
                   else if (!strcmp(buf, "data"))
                     {
                        if (t->cb.data)
                          PARSE_ERROR("Data already specified");
                        p->sm = SM_TRANSITION_CB_DATA_PROP;
                     }
                   else
                     PARSE_ERROR("Invalid token [%s]", buf);
                   break;

                case SM_TRANSITION_CB_FUNC_PROP:
                   t->cb.func = eina_stringshare_add_length(buf, k);
                   p->sm = SM_TRANSITION_CB;
                   break;

                case SM_TRANSITION_CB_DATA_PROP:
                   t->cb.data = eina_stringshare_add_length(buf, k);
                   p->sm = SM_TRANSITION_CB;
                   break;

                case SM_TRANSITION_START:
                   t = transit_new(buf, k);
                   if (eina_hash_find(f->transitions, t->name))
                     PARSE_ERROR("Transition [%s] already defined", t->name);
                   eina_hash_add(f->transitions, t->name, t);
                   p->sm = SM_TRANSITION_FROM;
                   break;

                case SM_TRANSITION_FROM:
                   t->from = eina_stringshare_add_length(buf, k);
                   p->sm = SM_TRANSITION_TO;
                   break;

                case SM_TRANSITION_TO:
                   t->to = eina_stringshare_add_length(buf, k);
                   p->sm = SM_TRANSITION_START;
                   break;

                default:
                   break;
               }

             p->has_token = EINA_FALSE;
             p->stop_word = EINA_FALSE;
             k = 0;
          }
     }

   return p->parse;
fail:
   return NULL;
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

