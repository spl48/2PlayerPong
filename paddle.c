/**
 @file paddle.c
 @authors Sean Plane, John Kim
 @date 15/10/2018
 **/

#include <avr/io.h>
#include "system.h"
#include "tinygl.h"
#include "paddle.h"
#include "boing.h"


paddle_t go_left(paddle_t paddle)
{
    if (paddle.lpos.y > 0) {
        tinygl_draw_line (paddle.lpos, paddle.rpos, 0);
        paddle.lpos.y = paddle.lpos.y - 1;
        paddle.rpos.y = paddle.rpos.y - 1;
        tinygl_draw_line (paddle.lpos, paddle.rpos, 1);
    }
    return paddle;
}

paddle_t go_right(paddle_t paddle)
{
    if (paddle.rpos.y < TINYGL_HEIGHT-1) {
        tinygl_draw_line (paddle.lpos, paddle.rpos, 0);
        paddle.lpos.y = paddle.lpos.y + 1;
        paddle.rpos.y = paddle.rpos.y + 1;
        tinygl_draw_line (paddle.lpos, paddle.rpos, 1);
    }
    return paddle;
}

char ball_collides(boing_state_t game_ball, paddle_t paddle)
{
    // Get the size of the paddle
    int paddle_size = paddle.lpos.y - paddle.rpos.y;
    if(paddle_size < 0) paddle_size *= -1;

    // Get the min_y of the paddle
    int min_y = paddle.lpos.y;
    if(paddle.rpos.y < min_y) min_y = paddle.rpos.y;

    // max_y of the paddle
    int max_y = min_y + paddle_size;

    // Get position of the paddle
    int x = paddle.lpos.x;

    // Check every position of the paddle
    int y;
    for(y = min_y; y <= max_y; y++){
        if(game_ball.pos.x == x && game_ball.pos.y == y){
            // Collision!
            return 1;
        }
    }

    // No collision
    return 0;
}

paddle_t init_paddle()
{
    paddle_t paddle;
    paddle.lpos.x = TINYGL_WIDTH - 1;
    paddle.lpos.y = (TINYGL_HEIGHT / 2) - 1;
    paddle.rpos.x = TINYGL_WIDTH - 1;
    paddle.rpos.y = (TINYGL_HEIGHT / 2) + 1;
    return paddle;
}

paddle_t init_other_paddle()
{
    paddle_t paddle;
    paddle.lpos.x = 0;
    paddle.lpos.y = (TINYGL_HEIGHT / 2) - 1;
    paddle.rpos.x = 0;
    paddle.rpos.y = (TINYGL_HEIGHT / 2) + 1;
    return paddle;
}
