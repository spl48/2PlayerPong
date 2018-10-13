#include <avr/io.h>
#include <stdbool.h>
#include "system.h"
#include "boing.h"
#include "pacer.h"
#include "tinygl.h"
#include "navswitch.h"
#include "paddle.h"
#include "ir_serial.h"
#include "ir_uart.h"
#include "../fonts/font3x5_1.h"

#define LOOP_RATE 200
#define MESSAGE_RATE 10


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
    //int playerNo = 1;
    bool startScreen = true;
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



    navswitch_init ();


    //ir_serial_init ();
    ir_uart_init ();


    //START SCREEN---------------------

    tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
    tinygl_text ("CLICK TO PLAY ");
    while (startScreen == true)
    {
        pacer_wait ();

        tinygl_update ();
        navswitch_update ();


        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            startScreen = false;
            tinygl_clear ();
            //set to player 1
            //set startscreen other player to false
        }
    }

    //MAIN GAME--------------------

    tinygl_draw_line (paddle.lpos, paddle.rpos, 1);
    tinygl_draw_line (otherPaddle.lpos, otherPaddle.rpos, 1);


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

        char opp_dir;

        navswitch_update ();

        if (navswitch_push_event_p (NAVSWITCH_NORTH))
        {
            paddle = go_left(paddle);
            ir_uart_putc ('r');
        }

        if (navswitch_push_event_p (NAVSWITCH_SOUTH))
        {
            paddle = go_right(paddle);
            ir_uart_putc ('l');
        }


        if (ir_uart_read_ready_p ()) {

           opp_dir = ir_uart_getc ();
           if (opp_dir == 'l')
           {
               otherPaddle = go_left(otherPaddle);
           } else if (opp_dir == 'r') {
               otherPaddle = go_right(otherPaddle);
           }

           // Process the received byte.
        }

        tinygl_update();



    }
}
