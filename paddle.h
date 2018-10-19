/**
 @file paddle.c
 @authors Sean Plane spl48, John Kim jki80
 @date 15/10/2018
 **/

#ifndef PADDLE_H
#define PADDLE_H

#include <avr/io.h>
#include "system.h"
#include "tinygl.h"
#include "boing.h"


typedef struct
{
    /** The x, y coordinate of the left pixel of the paddle. */
    tinygl_point_t lpos;
    /** The x, y coordinate of the right pixel of the paddle. */
    tinygl_point_t rpos;
} paddle_t;


/**Move the given paddle left. '
 * Takes in the current state of the paddle and returns a paddle with the new state after moving it left */
paddle_t go_left(paddle_t paddle);


/**Move the given paddle right.
 * Takes in the current state of the paddle and returns a paddle with the new state after moving it right */
paddle_t go_right(paddle_t paddle);

/** Checks if the ball is colliding with the given paddle.
 * Returns 1 if there is a collision, else returns 0 if there is no collision. */
char ball_collides(boing_state_t game_ball, paddle_t paddle);


/**Initialise the player's paddle. To the bottom and middle of the led matrix. */
paddle_t init_paddle(void);


/**Initialise the other player's paddle to the top and middle of the led matrix. */
paddle_t init_other_paddle(void);

#endif
