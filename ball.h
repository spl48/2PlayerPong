/**
 @file ball.h
 @authors Sean Plane spl48, John Kim jki80
 @date 18/10/2018
 @brief Controls the balls movement
 **/

#ifndef BALL_H
#define BALL_H

#include "boing.h"
#include "paddle.h"
#include "tinygl.h"

/** If the ball speed is greater than 1, increases the speed of the ball by 1 and returnds the new speed. */
int update_ball_speed(int ball_speed);


/** Rebounds the ball off the paddle. Changes the direction of the ball by 90 degrees and returns a ball with this new state (direction) */
boing_state_t rebound_off_paddle(boing_state_t ball,tinygl_point_t hops[]);


/** Rebounds the ball off the side of the led matrix. Changes the direction of the ball by 90 degrees and returns a ball with this new state (direction) */
boing_state_t rebound_off_side(boing_state_t ball, tinygl_point_t hops[]);


/** Moves the ball by one pixel in the direction the ball is travelling. */
boing_state_t move_ball(boing_state_t ball);


/** Checks if the ball should rebound off the paddle, the side of the led matrix, or continue moving in the same direction.
 * Returns a ball with the new state relating to these conditions */
boing_state_t update_ball (boing_state_t ball, paddle_t player1, paddle_t player2);

#endif
