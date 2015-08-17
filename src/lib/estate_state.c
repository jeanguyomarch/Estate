#include "estate_private.h"

#if 0
static void
_estate_state_cb_call(const Estate_State         *st,
                      const Estate_State_Cb_Type  cb_type)
{
   Eina_Inlist *l = EINA_INLIST_GET(st->cb[cb_type]);
   State_Cb_Wrapper *wrapper;

   EINA_INLIST_FOREACH(l, wrapper)
      wrapper->func(wrapper->data);
}
#endif


/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

EAPI Eina_Bool
estate_state_init(Estate_State             *st,
                  const char               *name,
                  const Estate_Transition **transitions,
                  unsigned int              transit_count)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(st, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(transitions, EINA_FALSE);

   unsigned int i;

   st->transit = eina_array_new(transit_count);
   if (EINA_UNLIKELY(!st->transit))
     {
        CRI("Failed to create Transitions array");
        goto fail;
     }

   for (i = 0; i < transit_count; ++i)
     eina_array_push(st->transit, transitions[i]);

   st->name = name;

   return EINA_TRUE;

fail:
   estate_state_deinit(st);
   return EINA_FALSE;
}

EAPI void
estate_state_deinit(Estate_State *st)
{
   if (!st) return;
   if (st->transit) eina_array_free(st->transit);
}

EAPI const char *
estate_state_name_get(const Estate_State *st)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(st, NULL);
   return st->name;
}

//EAPI void
//estate_state_cb_add(Estate_State   *st,
//                    Estate_Cb_Type  cb_type,
//                    Estate_Cb       func,
//                    void           *data)
//{
//   EINA_SAFETY_ON_NULL_RETURN(st);
//   EINA_SAFETY_ON_NULL_RETURN(func);
//
//   State_Cb_Wrapper *s;
//   Eina_Inlist *l;
//
//   s = malloc(sizeof(*s));
//   if (EINA_UNLIKELY(!s))
//     {
//        CRI("Failed to allocate State_Cb_Wrapper");
//        return;
//     }
//   s->func = func;
//   s->data = data;
//
//  // if (st->cb[cb_type] == NULL)
//  //   l = eina_inlist_append(NULL, EINA_INLIST_GET(s));
//  // else
//  //   l = eina_inlist_append(EINA_INLIST_GET(st->cb[cb_type]), EINA_INLIST_GET(s));
//}

