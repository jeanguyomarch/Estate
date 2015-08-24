#include "estate_private.h"

Estate_Mempool *
_estate_mempool_new(size_t    mem,
                    Eina_Bool zero)
{
   EINA_SAFETY_ON_TRUE_RETURN_VAL(mem == 0, NULL);

   Estate_Mempool *mp;
   size_t mem_augmented;

   /* Includes the Mempool structure */
   mem_augmented = mem + sizeof(Estate_Mempool);

   mp = malloc(mem_augmented);
   if (EINA_UNLIKELY(!mp))
     {
        CRI("Failed to create mempool (size: %zu)", mem_augmented);
        return NULL;
     }

   mp->base = mp + sizeof(Estate_Mempool);
   mp->ptr = mp->base;
   mp->mem = mem;

   if (zero)
     memset(mp->base, 0, mem);

   return mp;
}

void
_estate_mempool_free(Estate_Mempool *mp)
{
   EINA_SAFETY_ON_NULL_RETURN(mp);
   free(mp);
}

void *
_estate_mempool_

