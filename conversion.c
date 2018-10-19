/**
 @file conversion.c
 @authors Sean Plane spl48, John Kim jki80
 @date 18/10/2018
 @brief Handles all the conversions of objects positions to char so that they can be sent
 to the other board, and the other board can convert back to a position.
 **/

#include <avr/io.h>
#include "system.h"
#include "tinygl.h"
#include "conversion.h"
#include "paddle.h"
#include "boing.h"

/**
 * Converts the recieved char from the other board into a point object containing an x and y coordinate of the other players paddle.
 **/
tinygl_point_t convert_char_to_paddle(char packet)
{
    int y1 = packet % TINYGL_HEIGHT;
    int y2 = packet / TINYGL_HEIGHT;

    tinygl_point_t pos;
    pos.x = TINYGL_HEIGHT - 1 - y1; // lpos y when x = 0
    pos.y = TINYGL_HEIGHT - 1 - y2; // rpos y when x = 0
    return pos;
}

/**
 * Converts the recieved char from the other board into a point object containing an x and y coordinate of the ball.
 **/
tinygl_point_t convert_char_to_ball(char packet)
{
    packet -= 128;
    int x = packet % TINYGL_WIDTH;
    int y = packet / TINYGL_WIDTH;

    tinygl_point_t pos;
    pos.x = TINYGL_WIDTH - 1 - x;
    pos.y = TINYGL_HEIGHT - 1 - y;
    return pos;
}

/**
 * Takes the current state of the given paddle and converts it into a char to be sent to the other board
 **/
unsigned char convert_paddle_to_char(paddle_t paddle_state)
{
    unsigned char packet = 0;
    unsigned char position = paddle_state.lpos.y + (paddle_state.rpos.y * TINYGL_HEIGHT);
    packet += position;
    return packet;
}

/**
 * Takes the current state of the given ball and converts it into a char to be sent to the other board
 **/
unsigned char convert_ball_to_char(boing_state_t game_ball)
{
    unsigned char packet = 128;
    unsigned char position = game_ball.pos.x + (game_ball.pos.y * TINYGL_WIDTH);
    packet += position;
    return packet;
}
