#include "estate_cc.h"

static Eina_Bool
_each_states_gi_cb(const void *ctr   EINA_UNUSED,
                   void       *data,
                   void       *fdata)
{
   FILE *f = fdata;
   State *s = data;
   Eina_Stringshare *str[] = {
      s->enterer.func,
      s->exiter.func,
      s->transition.func
   };
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

Eina_Bool
estate_cc_out_gi(Eina_List  *parse,
                 const char *file,
                 const char *include)
{
   FILE *f = stdout;
   Eina_List *l;
   Fsm *fsm;

   /* Open file */
   if (file)
     f = fopen(file, "w");
   if (EINA_UNLIKELY(!f))
     {
        CRI("Failed to open file \"%s\"", file);
        return EINA_FALSE;
     }

   fprintf(f, "#include <Estate.h>\n\n");
   EINA_LIST_FOREACH(parse, l, fsm)
     {
        eina_array_foreach(fsm->states, _each_states_gi_cb, f);
     }
   fprintf(f, "#include \"%s\"\n\n", include);

   fflush(f);
   if (file)
     fclose(f);

   return EINA_TRUE;
}

