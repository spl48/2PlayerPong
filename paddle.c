/**
 @file paddle.c
 @authors Sean Plane spl48, John Kim jki80
 @date 15/10/2018
 **/

#include <avr/io.h>
#include "system.h"
#include "tinygl.h"
#include "paddle.h"
#include "boing.h"

#define PADDLE_SIZE 3 // Size of the paddles in number of pixels.

/**Move the given paddle left. '
 * Takes in the current state of the paddle and returns a paddle with the new state after moving it left*/
paddle_t go_left(paddle_t paddle)
{
    if (paddle.lpos.y > 0)
    {
        tinygl_draw_line (paddle.lpos, paddle.rpos, 0);
        paddle.lpos.y = paddle.lpos.y - 1;
        paddle.rpos.y = paddle.rpos.y - 1;
        tinygl_draw_line (paddle.lpos, paddle.rpos, 1);
    }
    return paddle;
}


/**Move the given paddle right.
 * Takes in the current state of the paddle and returns a paddle with the new state after moving it right */
paddle_t go_right(paddle_t paddle)
{
    if (paddle.rpos.y < TINYGL_HEIGHT-1)
    {
        tinygl_draw_line (paddle.lpos, paddle.rpos, 0);
        paddle.lpos.y = paddle.lpos.y + 1;
        paddle.rpos.y = paddle.rpos.y + 1;
        tinygl_draw_line (paddle.lpos, paddle.rpos, 1);
    }
    return paddle;
}


/**
 * Checks if the ball is colliding with the given paddle.
 * Returns 1 if there is a collision, else returns 0 if there is no collision.
 **/
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
        if(game_ball.pos.x == x && game_ball.pos.y == y) // Collision!
        {
            return 1;
        }
    }

    // No collision
    return 0;
}


/**Initialise the player's paddle. To the bottom and middle of the led matrix. */
paddle_t init_paddle()
{
    paddle_t paddle;
    paddle.lpos.x = TINYGL_WIDTH - 1;
    if (PADDLE_SIZE % 2 != 0) // If paddle size is odd, can just extend even amount on either side of centre pixel.
    {
        paddle.lpos.y = (TINYGL_HEIGHT / 2) - (PADDLE_SIZE / 2);
    } else { // If paddle size is even, increase left position by 1.
        paddle.lpos.y = ((TINYGL_HEIGHT / 2) - (PADDLE_SIZE / 2)) + 1;
    }
    paddle.rpos.x = TINYGL_WIDTH - 1;
    paddle.rpos.y = (TINYGL_HEIGHT / 2) + (PADDLE_SIZE / 2);
    return paddle;
}


/**Initialise the other player's paddle to the top and middle of the led matrix. */
paddle_t init_other_paddle()
{
    paddle_t paddle;
    paddle.lpos.x = 0;
    if (PADDLE_SIZE % 2 != 0) // If paddle size is odd, can just extend even amount on either side of centre pixel.
    {
        paddle.lpos.y = (TINYGL_HEIGHT / 2) - (PADDLE_SIZE / 2);
    } else { // If paddle size is even, increase left position by 1.
        paddle.lpos.y = ((TINYGL_HEIGHT / 2) - (PADDLE_SIZE / 2)) + 1;
    }
    paddle.rpos.x = 0;
    paddle.rpos.y = (TINYGL_HEIGHT / 2) + (PADDLE_SIZE / 2);
    return paddle;
}
