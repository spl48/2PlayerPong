/**
 @file game.c
 @authors Sean Plane spl48, John Kim jki80
 @date 15/10/2018
 @brief A multiplayer pong game that mirrors on both boards.
 **/

#include <avr/io.h>
#include <stdbool.h>
#include "system.h"
#include "boing.h"
#include "pacer.h"
#include "tinygl.h"
#include "navswitch.h"
#include "paddle.h"
#include "conversion.h"
#include "ball.h"
#include "ir_serial.h"
#include "ir_uart.h"
#include "button.h"
#include "../fonts/font3x5_1.h"

#define LOOP_RATE 200
#define MESSAGE_RATE 20

int is_host = 0;  // If 1, the board is the host of the game, 0 otherwise.


/**
 * Runs the start screen that allows the players to choose the host of the game.
 **/
void display_start_screen(bool start_screen)
{
    tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
    tinygl_text ("CLICK TO PLAY ");

    button_init();

    while (start_screen == true)
    {
        pacer_wait ();
        button_update ();
        tinygl_update ();
        navswitch_update ();

        if(button_push_event_p(0))
        {
            is_host = 1;
            tinygl_text ("HOST ");
        }

        if (navswitch_push_event_p(NAVSWITCH_PUSH) && is_host) //Send a signal 't' when the host presses the nav switch to tell the other board to start the game
        {
            start_screen = false;
            tinygl_clear ();
            ir_uart_putc ('t');
        }

        //If recieve ir signal 't' from other board stop start screen and go to main game
        if (ir_uart_read_ready_p ())
        {
           char temp = ir_uart_getc ();
           if (temp == 't')
           {
               start_screen = false;
               tinygl_clear ();
           }
       }
    }
}


/** Displays the corresponding win or lose text depending on whether the recieved packet char is a 'w' or 'l'. */
void display_win_lose(char packet)
{
    tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
    if (packet == 'w') // This board lost
    {
        tinygl_text("YOU LOSE");
    } else { // This board won
        tinygl_text("YOU WIN");
    }
}


/** Keeps the board in the end game state until the navswitch is pushed.
 * Once navswitch is pushed, clears all tinygl, sets host to 0, and goes back to main so that another game can be played. */
void end_game(void)
{
    while(!navswitch_push_event_p (NAVSWITCH_PUSH))
    {
        pacer_wait();
        navswitch_update();
        tinygl_update();
    }
    tinygl_clear();
    is_host = 0;
    main();
}


/** Moves the ball and checks if ball is out of bounds and therefore the game is over.
 * Else returns the new state of the ball by calling update_ball */
boing_state_t increment_ball (boing_state_t ball, paddle_t paddle, paddle_t other_paddle) {
    ball = move_ball(ball);
    if (ball.pos.x > TINYGL_WIDTH - 1 || ball.pos.x < 0) { // Ball out of bounds, Game-over
        if (ball.pos.x > TINYGL_WIDTH - 1)
        {
            display_win_lose('l');
            ir_uart_putc ('w');
        } else {
            display_win_lose('w');
            ir_uart_putc ('l');
        }
        end_game();
    } else {
        ball = update_ball(ball, paddle, other_paddle);
    }
    return ball;
}


/** Main game
 * Calls start screen, then runs the game */
int main (void)
{
    int tick = 0; //Counts up and controls when to update/move the ball.
    int ball_speed = 100; //Controlls the speed of the ball. Smaller number means faster ball speed
    boing_state_t ball = boing_init (0, 0, DIR_NE);

    system_init ();
    tinygl_init (LOOP_RATE);
    pacer_init (LOOP_RATE);
    navswitch_init ();
    ir_uart_init ();

    //Make both paddles -----
    paddle_t paddle;
    paddle_t other_paddle;
    paddle = init_paddle();
    other_paddle = init_other_paddle();

    //START SCREEN---------------------
    bool start_screen = true;
    display_start_screen(start_screen);

    //MAIN GAME--------------------
    tinygl_draw_line (paddle.lpos, paddle.rpos, 1);
    tinygl_draw_line (other_paddle.lpos, other_paddle.rpos, 1);

    while (1)
    {
        pacer_wait ();
        tick++;

        // BALL ----------------------
        if (tick >= ball_speed)
        {
            tick = 0;
            ball_speed = update_ball_speed(ball_speed);

            /* Erase previous position.  */
            tinygl_draw_point (ball.pos, 0);

            if(is_host == 1)
            {
                ball = increment_ball(ball, paddle, other_paddle);
                // Send the ball position to other board
                ir_uart_putc(convert_ball_to_char(ball));
            }
            tinygl_draw_point(ball.pos, 1);
        }

        navswitch_update ();

        // PADDLE MOVE ----------------
        if (navswitch_push_event_p (NAVSWITCH_NORTH))
        {
            paddle = go_left(paddle);
            // Send my paddle position packet
            ir_uart_putc(convert_paddle_to_char(paddle));

        }

        if (navswitch_push_event_p (NAVSWITCH_SOUTH))
        {
            paddle = go_right(paddle);
            // Send my paddle position packet
            ir_uart_putc(convert_paddle_to_char(paddle));
        }

        // Sync screens between boards i.e ball, and paddle positions.
        while (ir_uart_read_ready_p ())
        {
            unsigned char packet = ir_uart_getc();
            if (packet == 'w' || packet == 'l')
            {
                display_win_lose(packet);
                end_game();
            }
            if(is_host == 0 && packet >= 128)
            {
                // Ball
                tinygl_draw_point(ball.pos, 0);
                ball.pos = convert_char_to_ball(packet);
                tinygl_draw_point(ball.pos, 1);
            } else {
                // Enemy paddle
                tinygl_draw_line (other_paddle.lpos, other_paddle.rpos, 0);
                tinygl_point_t ycords = convert_char_to_paddle(packet);
                other_paddle.lpos.y = ycords.x;   //y1
                other_paddle.rpos.y = ycords.y;   //y2
                tinygl_draw_line (other_paddle.lpos, other_paddle.rpos, 1);
            }
        }
        tinygl_update();
    }
}
