#include <Estate.h>
#include "hello.est.c"

struct shopping_cart {
   unsigned int items_count; /* How many items are in the shopping cart */
};

/* Use the prototype below to export the loading function */
Estate_Machine *estate_fsm_hello_load(void) EINA_MALLOC;

Estate_Machine *
estate_fsm_hello_load(void)
{
   return _estate_fsm_hello_load();
}

static void
_leave_house_cb(void                    *data       EINA_UNUSED,
                Estate_Cb_Type           type       EINA_UNUSED,
                const Estate_Transition *transition EINA_UNUSED)
{
   printf("Bye, House!\n");
}

static void
_burn_in_hell_cb(void                    *data,
                 Estate_Cb_Type           type       EINA_UNUSED,
                 const Estate_Transition *transition EINA_UNUSED)
{
   struct shopping_cart *shop_cart = data;
   printf("Oh sh*t, I'm in Hell right now... with %u items in my shopping "
          "card... #yolo\n", shop_cart->items_count);
}

static void
_say_hello_cb(void                    *data       EINA_UNUSED,
              Estate_Cb_Type           type       EINA_UNUSED,
              const Estate_Transition *transition EINA_UNUSED)
{
   printf("Hello, World!\n");
}


int
main(void)
{
   Estate_Machine *fsm;
   struct shopping_cart *cart;

   estate_init(); /* Always initialize Estate */

   /* Create the shopping cart */
   cart = malloc(sizeof(*cart));
   cart->items_count = 666;

   /* Load the finite state machine */
   fsm = estate_fsm_hello_load();

   /* Register the shopping cart data */
   estate_machine_data_set(fsm, "ShoppingCart", cart);

   /* Scenario... */
   estate_machine_transition_do(fsm, "leave"); /* Leave the house */
   estate_machine_transition_do(fsm, "sayHello"); /* Say hello to a guy */
   estate_machine_transition_do(fsm, "goShopping"); /* Go shopping */
   if (cart->items_count == 666)
     estate_machine_transition_do(fsm, "goToHell"); /* Go to Hell */
   else
     estate_machine_transition_do(fsm, "goHome"); /* Go home */

   /* If the previous state was 'Home', this is possible.
    * Otherwise, it will refuse. */
   estate_machine_transition_do(fsm, "leave");


   /* We are done with our little scenario */


   /* Release resources */
   free(cart);
   estate_shutdown(); /* Shutdown Estate */

   return 0;
}

