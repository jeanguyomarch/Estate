Estate
======

Estate is:
- a C library to create Finite State Machines (FSM)
- a descriptive language which generate FSM.

The estate language allows to describe an FSM with a specific, well-suited syntax.
The estate compiler (`estate_cc`) can generate C code, which uses the
Estate library that you can easily integrate within your implementation.

Have a look at the example below for a better understanding of what
can bring you Estate and how to use it efficiently.


Install
-------

This project uses standard autotools.
It requires the Eina library (search for Enlightenment Foundation Libraries - EFL).

Quick guide:

- `./autogen.sh`;
- `make`;
- `make install` (probably as root);
- `ldconfig` (probably as root) if you are using Linux.


Vim syntax support
------------------

Estate is supported by vim.
Look there: https://github.com/jeanguyomarch/vim-estate.


Example: hello.est
------------------

The model that is represented by a finite state machine is as follows:
- a person leaves his house;
- he says hello to someone;
- he goes shopping;
- he can:
  - go back to his place, and therefore may do the same routine another day *OR*
  - go to hell, and then it's over for him (pun intended).

We can define a finite state machine with:
- transitions:
  - _leave_: he leaves the house;
  - _sayHello_: he says hello;
  - _goShopping_: he goes shopping;
  - _goHome_: he goes back home;
  - _goToHell_: he goes to hell;
- states:
  - _Home_: its place;
  - _Walk_: he is in the street (out of his house);
  - _Chat_: he is chatting;
  - _Shop_: he is at its favorite supermarket;
  - _Hell_: he is so f.cked...


We wish to register events when some transition is activated or when
some specific states are entered our exited.
The implementation will be done in C, and as developer I have
planned to write some callback functions:
- `_leave_house_cb`: activated when the person leaves his house;
- `_say_hello_cb`: activated when the person says hello;
- `_burn_in_hell_cb`: activated when the person enters in Hell;

Furthermore, I want to pass a parameter to one callback: when the person
goes to hell, I want to retrieve the shopping cart he purchased earlier.
This data, of type `struct shopping_cart` will be registered with the
key "*ShoppingCart*".

Below is the Estate file you have to write:


```
/*
 * hello.est
 */

hello { // hello is the name of the finite state machine
   transitions {
      /* Describe the transitions */
      model {
         leave : Home > Walk;
         sayHello : Walk > Chat;
         goShopping : Chat > Shop;
         goHome : Shop > Home;
         goToHell : Shop > Hell;
      }

      /* Provide the callback '_say_hello_cb' to the transition 'sayHello' */
      sayHello {
         cb {
            func: _say_hello_cb;
         }
      }
   }

   states {
      /* Provide the callback '_leave_house_cb' to the state 'Home'.
       * It will be called when exiting the state 'Home' */
      Home @init { /* Enforce we start at the 'Home' state */
         exiter {
            func: _leave_house_cb;
         }
      }

      /* Provide the callback '_burn_in_hell_cb' to the state 'Hell'.
       * Register a user data with the key 'ShoppingCart'.
       * The callback will be called when entering the state 'Hell' */
      Hell {
         enterer {
            func: _burn_in_hell_cb;
            data: ShoppingCart;
         }
      }
   }
}

```

When you are done with your description, just generate the C code:

```
estate_cc  --output hello.est.c  --gc  hello.est
```

This will create a file named `hello.est.c` from your FSM `hello.est`.


Then, you want to generate a C boilerplate for what you have written. The boilerplate
allows you to start implementing immediatly instead of having to setup the
interaction between the pure C implementation and the code generated from the
estate file above.

```
estate_cc  --output hello.c  --gi hello.est.c  hello.est
```

Note that you have to specify the name of the C file generated earlier (`hello.est.c`).
Now you have a C file that looks like this:

```c
#include <Estate.h> // You need the Estate library
#include "hello.est.c" // The generated implementation of the finite state machine

/* Use this function to load the FSM 'hello' */
Estate_Machine *
estate_fsm_hello_load(void)
{
   /* If you want to add your own hooks, you can do it here.
    * _estate_fsm_hello_load() is generated */
   return _estate_fsm_hello_load();
}

static void
_say_hello_cb(void                    *data       EINA_UNUSED,
              Estate_Cb_Type           type       EINA_UNUSED,
              const Estate_Transition *transition EINA_UNUSED)
{
}

static void
_leave_house_cb(void                    *data       EINA_UNUSED,
                Estate_Cb_Type           type       EINA_UNUSED,
                const Estate_Transition *transition EINA_UNUSED)
{
}

static void
_burn_in_hell_cb(void                    *data       EINA_UNUSED,
                 Estate_Cb_Type           type       EINA_UNUSED,
                 const Estate_Transition *transition EINA_UNUSED)
{
}
```

If you are not familiar with the `Eina` library, `EINA_UNUSED` just mean that for
now the argument is not used in your implementation. You must remove it if you
use the corresponding argument.


Finally, let's implement:

```c
#include <Estate.h> // You need the Estate library
#include "hello.est.c" // The generated implementation of the finite state machine

struct shopping_cart {
   unsigned int items_count; /* How many items are in the shopping cart */
};

/* Use this function to load the FSM 'hello' */
Estate_Machine *
estate_fsm_hello_load(void)
{
   /* If you want to add your own hooks, you can do it here.
    * _estate_fsm_hello_load() is generated */
   return _estate_fsm_hello_load();
}

static void
_say_hello_cb(void                    *data       EINA_UNUSED,
              Estate_Cb_Type           type       EINA_UNUSED,
              const Estate_Transition *transition EINA_UNUSED)
{
   printf("Hello, World!\n");
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
```

You can just try it out:

```
gcc -Wall -o hello hello.c
./hello
```

And you will get the following output:

```
Bye, House!
Hello, World!
Oh sh*t, I'm in Hell right now... with 666 items in my shopping card... #yolo
ERR<91047>:estate estate_machine.c:221 estate_machine_transition_do() Could not find transition "leave" at current state (Hell)
```

Note that the error on the last line is normal because the person was in Hell:
it cannot do a transition back home, because it was not planned when describing
the finite state machine.


License
-------

The MIT License (MIT)

Copyright (c) 2015 Jean Guyomarc'h

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

