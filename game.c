/**
 @file game.c
 @authors Sean Plane, John Kim
 @date 15/10/2018
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

int is_host = 0;

boing_state_t boing_update_ball (boing_state_t state, paddle_t player1, paddle_t player2);

/**
 * Runs the start screen that allows the players to choose the host of the game.
 **/
void start_screen(bool startScreen)
{
    tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
    tinygl_text ("CLICK TO PLAY ");

    button_init();

    while (startScreen == true)
    {
        pacer_wait ();
        button_update ();
        tinygl_update ();
        navswitch_update ();

        if(button_push_event_p(0)){
            is_host = 1;
            tinygl_text ("HOST MODE");
        }

        if (navswitch_push_event_p(NAVSWITCH_PUSH) && is_host) {
            startScreen = false;
            tinygl_clear ();
            ir_uart_putc ('t');
        }

        //If recieve ir signal from other board stop start screen and go to main game
        if (ir_uart_read_ready_p ()) {
           char opp_dir = ir_uart_getc ();
           if (opp_dir == 't')
           {
               startScreen = false;
               tinygl_clear ();
           }
       }
    }
}

void display_win_lose(char packet)
{
    tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
    if (packet == 'w')
    {
        tinygl_text("YOU LOSE");
    } else {
        tinygl_text("YOU WIN");
    }
}

void end_game()
{
    while(!navswitch_push_event_p (NAVSWITCH_PUSH)) {
        pacer_wait();
        navswitch_update();
        tinygl_update();
    }
    tinygl_clear();
    is_host = 0;
    main();
}

int main (void)
{
    int tick = 0;
    int ball_speed = 100; //Controlls the speed of the ball. Smaller number means faster ball speed
    boing_state_t ball = boing_init (0, 0, DIR_NE);

    system_init ();
    tinygl_init (LOOP_RATE);
    pacer_init (LOOP_RATE);
    navswitch_init ();
    ir_uart_init ();

    //Make both paddles -----
    paddle_t paddle;
    paddle_t otherPaddle;
    paddle = init_paddle();
    otherPaddle = init_other_paddle();

    //START SCREEN---------------------
    bool startScreen = true;
    start_screen(startScreen);

    //MAIN GAME--------------------
    tinygl_draw_line (paddle.lpos, paddle.rpos, 1);
    tinygl_draw_line (otherPaddle.lpos, otherPaddle.rpos, 1);

    while (1)
    {
        pacer_wait ();
        tick++;

        if (tick >= ball_speed)
        {
            tick = 0;
            ball_speed = update_ball_speed(ball_speed);

            /* Erase previous position.  */
            tinygl_draw_point (ball.pos, 0);

            /* Check for collision; if so reverse direction.  */
            if(is_host == 1)
            {
                ball = move_ball(ball);
                if (ball.pos.x > TINYGL_WIDTH - 1 || ball.pos.x < 0) { // Game-over
                    if (ball.pos.x > TINYGL_WIDTH - 1) {
                        display_win_lose('l');
                        ir_uart_putc ('w');
                    } else {
                        display_win_lose('w');
                        ir_uart_putc ('l');
                    }
                    end_game();
                } else {
                    ball = update_ball(ball, paddle, otherPaddle);
                }

                // Send the ball position to other board if host
                if(is_host == 1)
                {
                    ir_uart_putc(convertBallToChar(ball));
                }
            }
            tinygl_draw_point(ball.pos, 1);
        }

        navswitch_update ();

        if (navswitch_push_event_p (NAVSWITCH_NORTH))
        {
            paddle = go_left(paddle);
            // Send my paddle position packet
            ir_uart_putc(convertPaddleToChar(paddle));

        }

        if (navswitch_push_event_p (NAVSWITCH_SOUTH))
        {
            paddle = go_right(paddle);
            // Send my paddle position packet
            ir_uart_putc(convertPaddleToChar(paddle));
        }

        while (ir_uart_read_ready_p ()) {

            unsigned char packet = ir_uart_getc();
            if (packet == 'w' || packet == 'l')
            {
                display_win_lose(packet);
                end_game();
            }
            if(is_host == 0 && packet >= 128) {
                // Ball
                tinygl_draw_point(ball.pos, 0);
                ball.pos = convertCharToBall(packet);
                tinygl_draw_point(ball.pos, 1);
            } else {
                // Enemy paddle
                tinygl_draw_line (otherPaddle.lpos, otherPaddle.rpos, 0);
                tinygl_point_t ycords = convertCharToPaddle(packet);
                otherPaddle.lpos.y = ycords.x;   //y1
                otherPaddle.rpos.y = ycords.y;   //y2
                tinygl_draw_line (otherPaddle.lpos, otherPaddle.rpos, 1);
            }
        }
        tinygl_update();
    }
}
