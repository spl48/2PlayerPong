/**
 @file conversion.h
 @authors Sean Plane spl48, John Kim jki80
 @date 18/10/2018
  @brief Handles all the conversions of objects positions to char so that they can be sent
 to the other board, and the other board can convert back to a position.
 **/

#ifndef CONVERSION_H
#define CONVERSION_H

#include <avr/io.h>
#include "system.h"
#include "tinygl.h"
#include "paddle.h"
#include "boing.h"

/**
 * Converts the recieved char from the other board into a point object containing an x and y coordinate of the other players paddle.
 **/
tinygl_point_t convert_char_to_paddle(char packet);

/**
 * Converts the recieved char from the other board into a point object containing an x and y coordinate of the ball.
 **/
tinygl_point_t convert_char_to_ball(char packet);

/**
 * Takes the current state of the given paddle and converts it into a char to be sent to the other board
 **/
unsigned char convert_paddle_to_char(paddle_t paddle_state);

/**
 * Takes the current state of the given ball and converts it into a char to be sent to the other board
 **/
unsigned char convert_ball_to_char(boing_state_t game_ball);

#endif
