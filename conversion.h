/**
 @file conversion.h
 @authors Sean Plane, John Kim
 @date 18/10/2018
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
tinygl_point_t convertCharToPaddle(char packet);

/**
 * Converts the recieved char from the other board into a point object containing an x and y coordinate of the ball.
 **/
tinygl_point_t convertCharToBall(char packet);

/**
 * Takes the current state of the given paddle and converts it into a char to be sent to the other board
 **/
unsigned char convertPaddleToChar(paddle_t paddle_state);

/**
 * Takes the current state of the given ball and converts it into a char to be sent to the other board
 **/
unsigned char convertBallToChar(boing_state_t game_ball);

#endif
