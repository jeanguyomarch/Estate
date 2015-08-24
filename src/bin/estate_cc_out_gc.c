#include "estate_cc.h"

static Eina_Bool
_each_transitions_gc_alloc_cb(const Eina_Hash *hash   EINA_UNUSED,
                              const void      *key    EINA_UNUSED,
                              void            *data,
                              void            *fdata)
{
   Fsm_Wrapper *wrap = fdata;
   Transit *t = data;

   fprintf(wrap->f,
           "   t_%s = estate_transition_new(fsm);\n"
           "   if (EINA_UNLIKELY(t_%s == NULL)) goto fsm_fail;\n",
           t->name, t->name);

   return EINA_TRUE;
}

static Eina_Bool
_each_states_gc_alloc_cb(const Eina_Hash *hash   EINA_UNUSED,
                         const void      *key    EINA_UNUSED,
                         void            *data,
                         void            *fdata)
{
   Fsm_Wrapper*wrap = fdata;
   State *s = data;

   fprintf(wrap->f,
           "   s_%s = estate_state_new(fsm);\n"
           "   if (EINA_UNLIKELY(s_%s == NULL)) goto fsm_fail;\n",
           s->name, s->name);

   return EINA_TRUE;
}

static Eina_Bool
_each_transitions_gc_declare_cb(const Eina_Hash *hash   EINA_UNUSED,
                                const void      *key    EINA_UNUSED,
                                void            *data,
                                void            *fdata)
{
   Fsm_Wrapper *wrap = fdata;
   Transit *t = data;

   fprintf(wrap->f, "   Estate_Transition *t_%s = NULL;\n", t->name);

   return EINA_TRUE;
}

static Eina_Bool
_each_states_gc_declare_cb(const Eina_Hash *hash   EINA_UNUSED,
                           const void      *key    EINA_UNUSED,
                           void            *data,
                           void            *fdata)
{
   Fsm_Wrapper*wrap = fdata;
   State *s = data;

   fprintf(wrap->f, "   Estate_State *s_%s = NULL;\n", s->name);

   return EINA_TRUE;
}

static Eina_Bool
_each_transitions_gc_print_from_cb(const Eina_Hash *hash   EINA_UNUSED,
                                   const void      *key    EINA_UNUSED,
                                   void            *data,
                                   void            *fdata)
{
   Fsm_Wrapper *wrap = fdata;
   Transit *t = data;

   if (t->from == wrap->cstate->name)
     fprintf(wrap->f, "           t_%s,\n", t->name);

   return EINA_TRUE;
}

static Eina_Bool
_each_states_gc_init_cb(const Eina_Hash *hash  EINA_UNUSED,
                        const void      *key   EINA_UNUSED,
                        void            *data,
                        void            *fdata)
{
   Fsm_Wrapper *wrap = fdata;
   State *s = data;

   fprintf(wrap->f,
           "     {\n"
           "        const Estate_Transition *trs[] = {\n");
   wrap->cstate = s;
   eina_hash_foreach(wrap->fsm->transitions, _each_transitions_gc_print_from_cb, wrap);
   fprintf(wrap->f,
           "           NULL\n"
           "        };\n"
           "        chk = estate_state_init(s_%s, \"%s\", trs, EINA_C_ARRAY_LENGTH(trs) - 1,\n"
           "                                %s, %s, %s, %s);\n"
           "        if (EINA_UNLIKELY(!chk)) goto fsm_fail;\n"
           "        chk = estate_machine_state_add(fsm, s_%s);\n"
           "        if (EINA_UNLIKELY(!chk)) goto fsm_fail;\n"
           "     }\n"
           "\n",
           s->name, s->name,
           s->enterer.func ?: "NULL", s->enterer.data ?: "NULL",
           s->exiter.func ?: "NULL", s->exiter.data ?: "NULL",
           s->name);

   return EINA_TRUE;
}

static Eina_Bool
_each_transitions_gc_init_cb(const Eina_Hash *hash  EINA_UNUSED,
                             const void      *key   EINA_UNUSED,
                             void            *data,
                             void            *fdata)
{
   Fsm_Wrapper *wrap = fdata;
   Transit *t = data;

   fprintf(wrap->f,
           "   chk = estate_transition_init(t_%s, \"%s\", s_%s, s_%s, %s, NULL);\n"
           "   if (EINA_UNLIKELY(!chk)) goto fsm_fail;\n"
           "   chk = estate_machine_transition_add(fsm, t_%s);\n"
           "   if (EINA_UNLIKELY(!chk)) goto fsm_fail;\n"
           "\n",
           t->name, t->name, t->from, t->to, t->cb.func ?: "NULL", t->name);

   return EINA_TRUE;
}

static void
_proto_func_common(FILE              *f,
                   Eina_Stringshare **str,
                   unsigned int       str_count)
{
   unsigned int i;
   for (i = 0; i < str_count; ++i)
     if (str[i])
       fprintf(f, "static void %s(void *data, Estate_Cb_Type type, "
               "const Estate_Transition *transition);\n",
               str[i]);
}

static Eina_Bool
_each_transitions_proto_func(const Eina_Hash *hash   EINA_UNUSED,
                             const void      *key    EINA_UNUSED,
                             void            *data,
                             void            *fdata)
{
   Fsm_Wrapper *wrap = fdata;
   Transit *t = data;
   _proto_func_common(wrap->f, &(t->cb.func), 1);
   return EINA_TRUE;
}

static Eina_Bool
_each_states_proto_func(const Eina_Hash *hash   EINA_UNUSED,
                        const void      *key    EINA_UNUSED,
                        void            *data,
                        void            *fdata)
{
   Fsm_Wrapper *wrap = fdata;
   State *s = data;
   const Eina_Stringshare *str[] = {
      s->enterer.func,
      s->exiter.func
   };
   _proto_func_common(wrap->f, str, EINA_C_ARRAY_LENGTH(str));
   return EINA_TRUE;
}


/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

Eina_Bool
estate_cc_out_gc(Eina_List  *parse,
                 const char *file)
{
   FILE *f = stdout;
   Eina_List *l;
   Fsm *fsm;
   Fsm_Wrapper wrap;

   if (file)
     f = fopen(file, "w");
   if (EINA_UNLIKELY(!f))
     {
        CRI("Failed to open file \"%s\"", file);
        return EINA_FALSE;
     }

   wrap.f = f;

   fprintf(f,
           "/*\n"
           " * This file has been generated by estate_cc.\n"
           " * DO NOT MODIFY MANUALLY!\n"
           " */\n"
           "\n"
           "#include <Estate.h>\n"
           "\n");

   EINA_LIST_FOREACH(parse, l, fsm)
     {
        wrap.fsm = fsm;

        /*=== Forward declarations of function callbacks */
        eina_hash_foreach(fsm->states, _each_states_proto_func, &wrap);
        eina_hash_foreach(fsm->transitions, _each_transitions_proto_func, &wrap);
        fprintf(f, "\n\n");

        /*=== Declarations of variables ===*/
        fprintf(f,
                "static Estate_Machine *\n"
                "_estate_fsm_%s_load(void)\n"
                "{\n"
                "   Estate_Machine *fsm = NULL;\n"
                "   Eina_Bool chk;\n",
                fsm->name);

        eina_hash_foreach(fsm->states, _each_states_gc_declare_cb, &wrap);
        eina_hash_foreach(fsm->transitions, _each_transitions_gc_declare_cb, &wrap);
        fprintf(f, "\n");

        /*=== Allocations ===*/
        fprintf(f,
                "   fsm = estate_machine_new(%u, %u);\n",
                eina_hash_population(fsm->states),
                eina_hash_population(fsm->transitions));

        eina_hash_foreach(fsm->states, _each_states_gc_alloc_cb, &wrap);
        eina_hash_foreach(fsm->transitions, _each_transitions_gc_alloc_cb, &wrap);
        fprintf(f, "\n");

        /*=== Init states & transitions ===*/
        eina_hash_foreach(fsm->states, _each_states_gc_init_cb, &wrap);
        eina_hash_foreach(fsm->transitions, _each_transitions_gc_init_cb, &wrap);
        fprintf(f, "\n");

        /*=== End of function ===*/
        fprintf(f,
                "   estate_machine_lock(fsm, s_%s);\n"
                "\n"
                "   return fsm;\n"
                "\n"
                "fsm_fail:\n"
                "   EINA_LOG_CRIT(\"Failed to create FSM \\\"%s\\\"\");\n"
                "   estate_machine_free(fsm);\n"
                "   return NULL;\n"
                "}\n"
                "\n",
                fsm->init, fsm->name);
     }

   fflush(f);
   if (file)
     fclose(f);
   return EINA_TRUE;
}

