#include <avr/io.h>
#include "system.h"
#include "boing.h"
#include "pacer.h"
#include "tinygl.h"
#include "navswitch.h"

#define LOOP_RATE 200

struct paddle
{
    tinygl_point_t lpos;
    tinygl_point_t rpos;
};

typedef struct paddle paddle_t;


static paddle_t go_left(paddle_t paddle) {
    tinygl_draw_line (paddle.lpos, paddle.rpos, 0);
    paddle.lpos.y = paddle.lpos.y - 1;
    paddle.rpos.y = paddle.rpos.y - 1;
    tinygl_draw_line (paddle.lpos, paddle.rpos, 1);
    return paddle;
}

static paddle_t go_right(paddle_t paddle) {
    tinygl_draw_line (paddle.lpos, paddle.rpos, 0);
    paddle.lpos.y = paddle.lpos.y + 1;
    paddle.rpos.y = paddle.rpos.y + 1;
    tinygl_draw_line (paddle.lpos, paddle.rpos, 1);
    return paddle;
}

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
    paddle.lpos.x = TINYGL_WIDTH - 1;
    paddle.lpos.y = (TINYGL_HEIGHT / 2) - 1;
    paddle.rpos.x = TINYGL_WIDTH - 1;
    paddle.rpos.y = (TINYGL_HEIGHT / 2) + 1;

    tinygl_draw_line (paddle.lpos, paddle.rpos, 1);

    navswitch_init ();

    while (1)
    {
        pacer_wait ();

        tick++;

        //tinygl_draw_point (balls[0].pos, tick % 2 < 1);

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
