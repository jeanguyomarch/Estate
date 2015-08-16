#include "estate_cc.h"

int _estate_cc_dom = -1;

int
main(int    argc,
     char **argv)
{
   int ret = EXIT_FAILURE;
   char const *file;
   Parser *p;
   Eina_List *parse;

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

   if (argc != 2)
     {
        ERR("Usage: %s file.est", argv[0]);
        goto log_shut;
     }
   file = argv[1];

   /* Setup parser */
   p = estate_cc_parser_new();
   if (EINA_UNLIKELY(!p))
     {
        CRI("Failed to create parser");
        goto log_shut;
     }
   if (EINA_UNLIKELY(!estate_cc_parser_file_set(p, file)))
     {
        ERR("Failed to set parser");
        goto parser_free;
     }

   parse = estate_cc_parser_parse(p);
   estate_cc_parser_parse_free(p);

   ret = EXIT_SUCCESS;

parser_free:
   estate_cc_parser_free(p);
log_shut:
   estate_cc_log_shutdown();
eina_shut:
   eina_shutdown();
end:
   return ret;
}

