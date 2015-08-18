#include "estate_cc.h"

typedef struct
{
   Fsm  *fsm;
   FILE *f;
} Fsm_Wrapper;

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
      s->transition.func
   };
   FILE *f = wrap->f;
   const int str_count = EINA_C_ARRAY_LENGTH(str);
   Eina_Stringshare *sh;
   int i;
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

   fprintf(wrap->f, "   Estate_Transition t_%s;\n", t->name);

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

   fprintf(wrap->f, "   Estate_State s_%s;\n", s->name);

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
           "   s_%s.name = %s;\n"
           "   s_%s.cb[ESTATE_CB_TYPE_ENTERER].func = %s;\n"
           "   s_%s.cb[ESTATE_CB_TYPE_ENTERER].data = %s;\n"
           "   s_%s.cb[ESTATE_CB_TYPE_EXITER].func = %s;\n"
           "   s_%s.cb[ESTATE_CB_TYPE_EXITER].data = %s;\n"
           "   s_%s.cb[ESTATE_CB_TYPE_TRANSITION].func = %s;\n"
           "   s_%s.cb[ESTATE_CB_TYPE_TRANSITION].data = %s;\n"
           "\n",
           s->name, s->name,
           s->name, s->enterer.func ?: "NULL",
           s->name, s->enterer.data ?: "NULL",
           s->name, s->exiter.func ?: "NULL",
           s->name, s->exiter.data ?: "NULL",
           s->name, s->transition.func ?: "NULL",
           s->name, s->transition.data ?: "NULL");

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
                "Eina_Bool\n"
                "estate_fsm_%s_load(void)\n"
                "{\n"
                "   const Eina_Bool ok = _estate_fsm_%s_load();\n"
                "   return ok;\n"
                "}\n"
                "\n",
                fsm->name, fsm->name, fsm->name);

        eina_hash_foreach(fsm->states, _each_states_gi_cb, &wrap);
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
                "{\n", fsm->name);

        eina_hash_foreach(fsm->states, _each_states_gc_init_cb, &wrap);
        eina_hash_foreach(fsm->transitions, _each_transit_gc_init_cb, &wrap);

        fprintf(f, "\n");

        eina_hash_foreach(fsm->states, _each_states_gc_fill_cb, &wrap);

        fprintf(f, "}\n\n");
     }

   fflush(f);
   if (file)
     fclose(f);
   return EINA_TRUE;
}

