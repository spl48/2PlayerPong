#include <avr/io.h>
#include "system.h"
#include "boing.h"
#include "pacer.h"
#include "tinygl.h"
#include "navswitch.h"
#include "paddle.h"

#define LOOP_RATE 200


/** Change the direction of a ball 90 degrees
    @param state current state
    @return new state with direction reversed.  */
boing_state_t boing_ninety (boing_state_t state)
{
    boing_dir_t newdir[] = {DIR_N, DIR_NW, DIR_N, DIR_SW,
                            DIR_N, DIR_SW, DIR_E, DIR_NW};

    state.dir = newdir[state.dir];
    return state;
}


int main (void)
{
    int tick;

    system_init ();
    tinygl_init (LOOP_RATE);

    pacer_init (LOOP_RATE);
    tick = 0;
    boing_state_t ball;
    ball = boing_init (0, 0, DIR_NE);

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

            tick = 0;

            /* Erase previous position.  */
                tinygl_draw_point (ball.pos, 0);

            /* Check for collision; if so reverse direction.  */
            ball = boing_update (ball);

            tinygl_draw_point (ball.pos, 1);

            int futureY;
            if (ball.pos.x == 3) {
                if (ball.dir == DIR_NW) {
                    futureY = ball.pos.y + 1;
                } else if (ball.dir == DIR_SW) {
                    futureY = ball.pos.y - 1;
                }
                if (futureY == paddle.lpos.y || futureY == paddle.rpos.y || futureY == paddle.lpos.y+1 || ball.pos.y == paddle.lpos.y || ball.pos.y == paddle.rpos.y || ball.pos.y == paddle.lpos.y+1) {
                    ball = boing_ninety(ball);
                } else {

                }
            }
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
