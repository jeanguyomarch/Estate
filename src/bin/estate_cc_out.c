#include "estate_cc.h"

typedef struct
{
   Fsm  *fsm;
   FILE *f;
} Fsm_Wrapper;

static void
_gi_cb(FILE *f,
       Eina_Stringshare **str,
       unsigned int       str_count)
{
   Eina_Stringshare *sh;
   unsigned int i;
   int l, len;

   for (i = 0; i < str_count; ++i)
     {
        sh = str[i];
        if (sh)
          {
             len = eina_stringshare_strlen(sh);
             fprintf(f,
                     "static void\n"
                     "%s(void                    *data,\n", sh);

             for (l = 0; l <= len; ++l)
               fprintf(f, " ");

             fprintf(f, "Estate_Cb_Type           type,\n");

             for (l = 0; l <= len; ++l)
               fprintf(f, " ");

             fprintf(f, "const Estate_Transition *transition)\n"
                     "{\n"
                     "\n"
                     "}\n"
                     "\n");
          }
     }
}

static Eina_Bool
_each_transitions_gi_cb(const Eina_Hash *hash   EINA_UNUSED,
                        const void      *key    EINA_UNUSED,
                        void            *data,
                        void            *fdata)
{
   Fsm_Wrapper *wrap = fdata;
   Transit *t = data;

   _gi_cb(wrap->f, &(t->cb.func), 1);

   return EINA_TRUE;
}

static Eina_Bool
_each_states_gi_cb(const Eina_Hash *hash   EINA_UNUSED,
                   const void      *key    EINA_UNUSED,
                   void            *data,
                   void            *fdata)
{
   Fsm_Wrapper *wrap = fdata;
   State *s = data;
   Eina_Stringshare *str[] = {
      s->enterer.func,
      s->exiter.func,
   };
   _gi_cb(wrap->f, str, EINA_C_ARRAY_LENGTH(str));

   return EINA_TRUE;
}

static Eina_Bool
_each_transit_gc_init_cb(const Eina_Hash *hash   EINA_UNUSED,
                         const void      *key    EINA_UNUSED,
                         void            *data,
                         void            *fdata)
{
   Fsm_Wrapper *wrap = fdata;
   Transit *t = data;

   fprintf(wrap->f, "   static Estate_Transition t_%s;\n", t->name);

   return EINA_TRUE;
}

static Eina_Bool
_each_states_gc_init_cb(const Eina_Hash *hash   EINA_UNUSED,
                        const void      *key    EINA_UNUSED,
                        void            *data,
                        void            *fdata)
{
   Fsm_Wrapper*wrap = fdata;
   State *s = data;

   fprintf(wrap->f, "   static Estate_State s_%s;\n", s->name);

   return EINA_TRUE;
}

static Eina_Bool
_each_states_gc_fill_cb(const Eina_Hash *hash  EINA_UNUSED,
                        const void      *key   EINA_UNUSED,
                        void            *data,
                        void            *fdata)
{
   Fsm_Wrapper *wrap = fdata;
   State *s = data;

   fprintf(wrap->f,
           "   s_%s.name = eina_stringshare_add_length(\"%s\", sizeof(\"%s\") - 1);\n"
           "   s_%s.cb[ESTATE_CB_TYPE_ENTERER].func = %s;\n"
           "   s_%s.cb[ESTATE_CB_TYPE_ENTERER].data = NULL;\n"
           "   s_%s.cb[ESTATE_CB_TYPE_EXITER].func = %s;\n"
           "   s_%s.cb[ESTATE_CB_TYPE_EXITER].data = NULL;\n"
           "   s_%s.transit = eina_array_new(1);\n"
           "\n",
           s->name, s->name, s->name,
           s->name, s->enterer.func ?: "NULL",
           s->name,
           s->name, s->exiter.func ?: "NULL",
           s->name,
           s->name);

   fprintf(wrap->f,
           "   eina_array_push(_fsm_%s.states, &s_%s);\n"
           "\n",
           wrap->fsm->name, s->name);

   return EINA_TRUE;
}

static Eina_Bool
_each_transitions_gc_gen_cb(const Eina_Hash *hash  EINA_UNUSED,
                            const void      *key   EINA_UNUSED,
                            void            *data,
                            void            *fdata)
{
   Fsm_Wrapper *wrap = fdata;
   Transit *t = data;

   fprintf(wrap->f,
           "   t_%s.name = eina_stringshare_add_length(\"%s\", sizeof(\"%s\") - 1);\n"
           "   t_%s.from = &s_%s;\n"
           "   t_%s.to = &s_%s;\n"
           "   t_%s.mach = &(_fsm_%s);\n"
           "   t_%s.cb.func = %s;\n"
           "   t_%s.cb.data = NULL;\n"
           "\n",
           t->name, t->name, t->name,
           t->name, t->from,
           t->name, t->to,
           t->name, wrap->fsm->name,
           t->name, t->cb.func ?: "NULL",
           t->name);

   fprintf(wrap->f,
           "   eina_array_push(t_%s.from->transit, &t_%s);\n"
           "   eina_array_push(_fsm_%s.transit, &t_%s);\n"
           "\n",
           t->name, t->name,
           wrap->fsm->name, t->name);

   return EINA_TRUE;
}


Eina_Bool
estate_cc_out_gi(Eina_List  *parse,
                 const char *file,
                 const char *include)
{
   FILE *f = stdout;
   Eina_List *l;
   Fsm_Wrapper wrap;
   Fsm *fsm;

   /* Open file */
   if (file)
     f = fopen(file, "w");
   if (EINA_UNLIKELY(!f))
     {
        CRI("Failed to open file \"%s\"", file);
        return EINA_FALSE;
     }

   wrap.f = f;

   fprintf(f, "#include <Estate.h>\n\n");
   EINA_LIST_FOREACH(parse, l, fsm)
     {
        wrap.fsm = fsm;
        fprintf(f,
                "ESTATE_MACHINE_DECLARE(%s);\n"
                "\n"
                "Eina_Bool estate_fsm_%s_load(void);\n"
                "\n"
                "Eina_Bool\n"
                "estate_fsm_%s_load(void)\n"
                "{\n"
                "   const Eina_Bool ok = _estate_fsm_%s_load();\n"
                "   return ok;\n"
                "}\n"
                "\n",
                fsm->name,
                fsm->name,
                fsm->name,
                fsm->name);

        eina_hash_foreach(fsm->states, _each_states_gi_cb, &wrap);
        eina_hash_foreach(fsm->transitions, _each_transitions_gi_cb, &wrap);
     }
   fprintf(f, "#include \"%s\"\n\n", include);

   fflush(f);
   if (file)
     fclose(f);

   return EINA_TRUE;
}

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
        fprintf(f,
                "static Eina_Bool\n"
                "_estate_fsm_%s_load(void)\n"
                "{\n"
                "   _fsm_%s.states = eina_array_new(%u);\n"
                "   _fsm_%s.transit = eina_array_new(%u);\n"
                "\n",
                fsm->name,
                fsm->name, eina_hash_population(fsm->states),
                fsm->name, eina_hash_population(fsm->transitions));


        eina_hash_foreach(fsm->states, _each_states_gc_init_cb, &wrap);
        eina_hash_foreach(fsm->transitions, _each_transit_gc_init_cb, &wrap);

        fprintf(f, "\n");

        eina_hash_foreach(fsm->states, _each_states_gc_fill_cb, &wrap);
        eina_hash_foreach(fsm->transitions, _each_transitions_gc_gen_cb, &wrap);

        fprintf(f,
                "   _fsm_%s.current_state = &s_%s\n"
                "\n"
                "   return EINA_TRUE;\n"
                "}\n"
                "\n",
                fsm->name, fsm->init);
     }

   fflush(f);
   if (file)
     fclose(f);
   return EINA_TRUE;
}

