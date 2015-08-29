#include "estate_cc.h"

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
                     "static int\n"
                     "%s(void                    *data       EINA_UNUSED,\n", sh);

             for (l = 0; l <= len; ++l)
               fprintf(f, " ");

             fprintf(f, "Estate_Cb_Type           type       EINA_UNUSED,\n");

             for (l = 0; l <= len; ++l)
               fprintf(f, " ");

             fprintf(f, "const Estate_Transition *transition EINA_UNUSED)\n"
                     "{\n"
                     "   return ESTATE_CB_OK;\n"
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

   fprintf(f,
           "#include <Estate.h>\n"
           "#include \"%s\"\n\n", include);

   EINA_LIST_FOREACH(parse, l, fsm)
     {
        wrap.fsm = fsm;
        fprintf(f,
                "/* Use the prototype below to export the loading function */\n"
                "Estate_Machine *estate_fsm_%s_load(void) EINA_MALLOC;\n"
                "\n"
                "Estate_Machine *\n"
                "estate_fsm_%s_load(void)\n"
                "{\n"
                "   return _estate_fsm_%s_load();\n"
                "}\n"
                "\n",
                fsm->name,
                fsm->name,
                fsm->name);

        eina_hash_foreach(fsm->states, _each_states_gi_cb, &wrap);
        eina_hash_foreach(fsm->transitions, _each_transitions_gi_cb, &wrap);
     }
   fprintf(f, "\n");

   fflush(f);
   if (file)
     fclose(f);

   return EINA_TRUE;
}

