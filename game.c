#include <avr/io.h>
#include "system.h"
#include "boing.h"
#include "pacer.h"
#include "tinygl.h"
#include "navswitch.h"
#include "paddle.h"
#include "ir_serial.h"

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
    paddle_t otherPaddle;
    paddle = init_paddle();
    otherPaddle = init_other_paddle();

    tinygl_draw_line (paddle.lpos, paddle.rpos, 1);
    tinygl_draw_line (otherPaddle.lpos, otherPaddle.rpos, 1);

    navswitch_init ();


    ir_serial_init ();

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

            //int futureY;
            if (ball.pos.x == 4) {
                if (ball.pos.y == paddle.lpos.y || ball.pos.y == paddle.rpos.y || ball.pos.y == paddle.lpos.y+1) {
                    tick += 100;
                    tinygl_draw_point (ball.pos, 1);
                    ball.pos.x--;

                    ball = boing_ninety(ball);
                    if (ball.dir == DIR_NW) {
                    ball.pos.y--;
                    } else if (ball.dir == DIR_SW) {
                        ball.pos.y++;
                    }
                    tinygl_draw_point (ball.pos, 1);
                } else {
                    //game over
                }
            }
        }



        // PADDLE --------------------------------------------------
        uint8_t data;
        int ret;

        navswitch_update ();

        if (navswitch_push_event_p (NAVSWITCH_NORTH))
        {
            paddle = go_left(paddle);
            ir_serial_transmit (1);
        }

        if (navswitch_push_event_p (NAVSWITCH_SOUTH))
        {
            paddle = go_right(paddle);
            ir_serial_transmit (2);
        }

        ret = ir_serial_receive (&data);
        if (ret == IR_SERIAL_OK)
        {
            if (data == 1)
                otherPaddle = go_left(otherPaddle);
            else if (data == 2)
                otherPaddle = go_right(otherPaddle);
        }
        tinygl_update ();



    }
}
