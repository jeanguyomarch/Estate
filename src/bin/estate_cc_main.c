#include "estate_cc.h"
#include <getopt.h>

int _estate_cc_dom = -1;

static const struct option _options[] =
{
     {"output",  required_argument, NULL, 'o'},
     {"verbose", no_argument,       NULL, 'v'},
     {"help",    no_argument,       NULL, 'h'},
     {NULL,      0,                 NULL,  0}
};

static void
_usage(char const *const prog,
       FILE       *      stream)
{
   fprintf(stream,
           "Usage: %s [OPTIONS] file\n"
           "\n"
           "With the following options:\n"
           "  -o|--output    specify the output file. stdout by default\n"
           "  -v|--verbose   cumulative, increase the verbosity level\n"
           "  -h|--help      display this message\n"
           "\n",
           prog);
}

int
main(int    argc,
     char **argv)
{
   int ret = EXIT_FAILURE;
   char const *input;
   char const *output = NULL;
   Parser *p;
   Eina_List *parse;
   int c;
   unsigned int verbosity = 0;

   /* Getopt */
   while (1)
     {
        c = getopt_long(argc, argv, "o:vh", _options, NULL);
        if (c == -1) break;
        switch (c)
          {
           case 'o':
              output = optarg;
              break;

           case 'v':
              verbosity++;
              break;

           case 'h':
              _usage(argv[0], stdout);
              return EXIT_SUCCESS;

           default:
              _usage(argv[0], stderr);
              goto end;
          }
     }
   if (argc - optind != 1)
     {
        _usage(argv[0], stderr);
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

