/**
 @file ball.h
 @authors Sean Plane spl48, John Kim jki80
 @date 18/10/2018
 **/

#ifndef BALL_H
#define BALL_H

 #include "boing.h"
 #include "paddle.h"
 #include "tinygl.h"

int update_ball_speed(int ball_speed);

boing_state_t rebound_off_paddle(boing_state_t ball,tinygl_point_t hops[]);

boing_state_t rebound_off_side(boing_state_t ball, tinygl_point_t hops[]);

boing_state_t move_ball(boing_state_t ball);

boing_state_t update_ball (boing_state_t ball, paddle_t player1, paddle_t player2);

#endif
