#include <avr/io.h>
#include "system.h"
#include "boing.h"
#include "pacer.h"
#include "tinygl.h"
#include "navswitch.h"
#include "paddle.h"

#define LOOP_RATE 200

int main (void)
{
    int tick;

    system_init ();
    tinygl_init (LOOP_RATE);

    pacer_init (LOOP_RATE);
    tick = 0;
    boing_state_t balls[1];
    balls[0] = boing_init (0, 1, DIR_NE);

    //PADDLE -----
    paddle_t paddle;
    paddle = init_paddle();

    tinygl_draw_line (paddle.lpos, paddle.rpos, 1);

    navswitch_init ();

    while (1)
    {
        pacer_wait ();

        tick++;

        if (tick >= 100)
        {
            int i;

            tick = 0;

            /* Erase previous position.  */
                tinygl_draw_point (balls[0].pos, 0);

            /* Check for collision; if so reverse direction.  */
            balls[i] = boing_update (balls[0]);

            tinygl_draw_point (balls[0].pos, 1);
        }



        // PADDLE --------------------------------------------------

        navswitch_update ();

        if (navswitch_push_event_p (NAVSWITCH_NORTH))
        {
            paddle = go_left(paddle);
        }

        if (navswitch_push_event_p (NAVSWITCH_SOUTH))
        {
            paddle = go_right(paddle);
        }

        tinygl_update ();



    }
}
