#include "estate_cc.h"
#include <getopt.h>

int _estate_cc_dom = -1;
static unsigned int _verbosity = 0;

static const struct option _options[] =
{
     {"output",   required_argument, NULL, 'o'},
     {"gc",       no_argument,       NULL, 'c'},
     {"gi",       required_argument, NULL, 'i'},
     {"pgf-tikz", optional_argument, NULL, 'k'},
     {"verbose",  no_argument,       NULL, 'v'},
     {"help",     no_argument,       NULL, 'h'},
     {NULL,       0,                 NULL,  0}
};

static void
_usage(char const *const prog,
       FILE       *      stream)
{
   fprintf(stream,
           "Usage: %s [--gc|--gi] [OPTIONS] file\n"
           "\n"
           "With the following options:\n"
           "  -o|--output <file>\n"
           "        specify the output file. stdout by default\n"
           "\n"
           "  -c|--gc\n"
           "        generates C code of states machines\n"
           "\n"
           "  -i|--gi <file>\n"
           "        generates a C boilerplate for callbacks\n"
           "\n"
           "  -k|--pgf-tikz [tikz style]\n"
           "        generates LaTeX (pgf-tikz) code\n"
           "\n"
           "  -v|--verbose          cumulative, increase the verbosity level\n"
           "\n"
           "  -h|--help             display this message\n"
           "\n",
           prog);
}

unsigned int
estate_cc_verbosity_get(void)
{
   return _verbosity;
}

int
main(int    argc,
     char **argv)
{
   int ret = EXIT_FAILURE;
   char const *input;
   char const *output = NULL;
   char const *include = NULL;
   Parser *p;
   Eina_List *parse;
   int c;
   Eina_Bool gc = EINA_FALSE;
   Eina_Bool gi = EINA_FALSE;
   Eina_Bool pgf_tikz = EINA_FALSE;

   /* Getopt */
   while (1)
     {
        c = getopt_long(argc, argv, "o:ci:k:vh", _options, NULL);
        if (c == -1) break;
        switch (c)
          {
           case 'o':
              output = optarg;
              break;

           case 'c':
              gc = EINA_TRUE;
              break;

           case 'i':
              gi = EINA_TRUE;
              include = optarg;
              break;

           case 'k':
              pgf_tikz = EINA_TRUE;
              break;

           case 'v':
              _verbosity++;
              break;

           case 'h':
              _usage(argv[0], stdout);
              return EXIT_SUCCESS;

           default:
              _usage(argv[0], stderr);
              goto end;
          }
     }

   /* Check for provided parameters */
   if (gc + gi + pgf_tikz != 1)
     {
        fprintf(stderr, "*** You must specify exactly one conversion format: "
                "--gc or --gi or -C\n");
        goto end;
     }

   /* Check for input */
   if (argc - optind != 1)
     {
        fprintf(stderr, "*** %s needs a manadatory argument\n", argv[0]);
        goto end;
     }
   input = argv[optind];

   /* Init Eina */
   if (EINA_UNLIKELY(!eina_init()))
     {
        EINA_LOG_CRIT("Failed to init Eina");
        goto end;
     }

   /* Init log */
   if (EINA_UNLIKELY(!estate_cc_log_init()))
     {
        EINA_LOG_CRIT("Failed to init log");
        goto eina_shut;
     }

   /* Setup parser */
   p = estate_cc_parser_new();
   if (EINA_UNLIKELY(!p))
     {
        CRI("Failed to create parser");
        goto log_shut;
     }
   if (EINA_UNLIKELY(!estate_cc_parser_file_set(p, input)))
     {
        ERR("Failed to set parser");
        goto parser_free;
     }

   /* Parse */
   parse = estate_cc_parser_parse(p);
   if (!parse)
     {
        ERR("Parsing of file \"%s\" failed", input);
        goto parser_free;
     }

   if (_verbosity >= 1)
     estate_cc_data_print(parse);

   estate_cc_check_states(parse);

   if (gi)
     estate_cc_out_gi(parse, output, include);
   if (gc)
     estate_cc_out_gc(parse, output);
   if (pgf_tikz)
     estate_cc_out_pgf_tikz(parse, output);

   ret = EXIT_SUCCESS;
   estate_cc_parser_parse_free(p);
parser_free:
   estate_cc_parser_free(p);
log_shut:
   estate_cc_log_shutdown();
eina_shut:
   eina_shutdown();
end:
   return ret;
}

