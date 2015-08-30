#include "estate_cc.h"

static int
_sort_cb(const void *d1,
         const void *d2)
{
   /* Yes... that's a little tricky. d1 and d2 are pointers on data,
    * which are pointer themselves... */
   const Transit *t1 = (const Transit *)(*(Transit * const *)d1);
   const Transit *t2 = (const Transit *)(*(Transit * const *)d2);

   /* Sort by STRINGSHARES and not strings!! */
   return (int)(t1->name - t2->name);
}

static Eina_Bool
_each_transitions_gc_from_cache_cb(const Eina_Hash *hash   EINA_UNUSED,
                                   const void      *key    EINA_UNUSED,
                                   void            *data,
                                   void            *fdata)
{
   Fsm_Wrapper *wrap = fdata;
   Transit *t = data;
   State *s = wrap->cstate;

   if (t->from == s->name)
     {
        s->trs = realloc(s->trs, (s->trs_count + 1) * sizeof(Transit *));
        s->trs[s->trs_count] = t;
        s->trs_count++;
     }

   return EINA_TRUE;
}


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
   Fsm_Wrapper *wrap = fdata;
   State *s = data;

   wrap->cstate = s;
   eina_hash_foreach(wrap->fsm->transitions, _each_transitions_gc_from_cache_cb, wrap);
   qsort(s->trs, s->trs_count, sizeof(Transit *), _sort_cb);

   fprintf(wrap->f,
           "   s_%s = estate_state_new(fsm, %u);\n"
           "   if (EINA_UNLIKELY(s_%s == NULL)) goto fsm_fail;\n",
           s->name, s->trs_count, s->name);

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

static unsigned int
_stringize_data(const char   *str,
                char         *buf,
                unsigned int  len)
{
   unsigned int plen;

   if (str)
     {
        plen = snprintf(buf, len, "\"%s\"", str);
        plen -= 2; /* Don't track the "" */
     }
   else
     {
        snprintf(buf, len, "NULL");
        plen = 0;
     }
   return plen;
}

static Eina_Bool
_each_states_gc_init_cb(const Eina_Hash *hash  EINA_UNUSED,
                        const void      *key   EINA_UNUSED,
                        void            *data,
                        void            *fdata)
{
   Fsm_Wrapper *wrap = fdata;
   State *s = data;
   char b1[128], b2[128];
   unsigned int i, len1, len2;

   len1 = _stringize_data(s->enterer.data, b1, sizeof(b1));
   len2 = _stringize_data(s->exiter.data, b2, sizeof(b2));

   fprintf(wrap->f, "     {\n");

   if (s->trs_count > 0)
     {
        fprintf(wrap->f, "        const Estate_Transition *trs[] = {\n");
        for (i = 0; i < s->trs_count; ++i)
          {
             fprintf(wrap->f, "           t_%s", s->trs[i]->name);
             if (i < s->trs_count - 1) fprintf(wrap->f, ",");
             fprintf(wrap->f, "\n");
          }
        fprintf(wrap->f,
                "        };\n"
                "        const unsigned int trs_count = EINA_C_ARRAY_LENGTH(trs);\n");
     }
   else
     fprintf(wrap->f,
             "        const Estate_Transition **trs = NULL;\n"
             "        const unsigned int trs_count = 0;\n");

   fprintf(wrap->f,
           "        enterer_ctor.func = %s;\n"
           "        enterer_ctor.key = %s;\n"
           "        enterer_ctor.key_len = %u;\n"
           "        exiter_ctor.func = %s;\n"
           "        exiter_ctor.key = %s;\n"
           "        exiter_ctor.key_len = %u;\n"
           "        chk = estate_state_init(s_%s, \"%s\", trs, trs_count,\n"
           "                                &enterer_ctor, &exiter_ctor);\n"
           "        if (EINA_UNLIKELY(!chk)) goto fsm_fail;\n"
           "        chk = estate_machine_state_add(fsm, s_%s);\n"
           "        if (EINA_UNLIKELY(!chk)) goto fsm_fail;\n"
           "     }\n"
           "\n",
           s->enterer.func ?: "NULL", b1, len1,
           s->exiter.func ?: "NULL", b2, len2,
           s->name, s->name,
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
   char b1[128], b2[128], b3[128];
   unsigned int len1, len2, len3;

   len1 = _stringize_data(t->cb.data, b1, sizeof(b1));
   len2 = _stringize_data(t->enterer.data, b2, sizeof(b2));
   len3 = _stringize_data(t->exiter.data, b3, sizeof(b3));

   fprintf(wrap->f,
           "   cb_ctor.func = %s;\n"
           "   cb_ctor.key = %s;\n"
           "   cb_ctor.key_len = %u;\n"
           "   enterer_ctor.func = %s;\n"
           "   enterer_ctor.key = %s;\n"
           "   enterer_ctor.key_len = %u;\n"
           "   exiter_ctor.func = %s;\n"
           "   exiter_ctor.key = %s;\n"
           "   exiter_ctor.key_len = %u;\n"
           "   chk = estate_transition_init(t_%s, \"%s\", s_%s, s_%s,\n"
           "                                &cb_ctor, &enterer_ctor, &exiter_ctor);\n"
           "   if (EINA_UNLIKELY(!chk)) goto fsm_fail;\n"
           "   chk = estate_machine_transition_add(fsm, t_%s);\n"
           "   if (EINA_UNLIKELY(!chk)) goto fsm_fail;\n"
           "\n",
           t->cb.func ?: "NULL", b1, len1,
           t->enterer.func ?: "NULL", b2, len2,
           t->exiter.func ?: "NULL", b3, len3,
           t->name, t->name, t->from, t->to,
           t->name);

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
       fprintf(f, "static int %s(void *data, Estate_Cb_Type type, "
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
                "   Eina_Bool chk;\n"
                "   Estate_Cb_Ctor cb_ctor, enterer_ctor, exiter_ctor;\n"
                "\n",
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

