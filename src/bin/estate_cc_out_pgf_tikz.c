#include "estate_cc.h"

Eina_Bool
estate_cc_out_pgf_tikz(Eina_List  *parse EINA_UNUSED,
                       const char *file)
{
   FILE *f = stdout;

   if (file)
     f = fopen(file, "w");
   if (EINA_UNLIKELY(!f))
     {
        CRI("Failed to open file \"%s\"", file);
        return EINA_FALSE;
     }

   fprintf(f,
           "%% Required packages:\n"
           "%%\\usepackage{pfg}\n"
           "%%\\usepackage{tikz}\n"
           "%%\\usetikzlibrary{arrows,automata}\n"
           "\n"
           "\\begin{tikzpicture}[shorten >=1pt,node distance=2cm,>=stealth',thick]\n");

   // TODO

   fprintf(f,
           "\\end{tikzpicture}\n");

   fflush(f);
   if (file) fclose(f);
   return EINA_TRUE;
}

