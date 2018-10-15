#include <avr/io.h>
#include <stdbool.h>
#include "system.h"
#include "boing.h"
#include "pacer.h"
#include "tinygl.h"
#include "navswitch.h"
#include "paddle.h"
#include "ir_serial.h"
#include "ir_uart.h"
#include "../fonts/font3x5_1.h"

#define LOOP_RATE 200
#define MESSAGE_RATE 10

int is_host =0;

static void button_init ()
{
    /* Initialise port to read button 1.  */

    /* TODO.  */
    DDRD &= ~(1 << 3);
}


static int button_pressed_p ()
{
    /* Return non-zero if button pressed_p.  */

    /* TODO.  */
    if ((PIND & (1 << 7)) == 0) {
        return 0;
    } else {
        return 1;
    }
}

tinygl_point_t convertCharToPaddle(char packet){
    int y1 = packet % TINYGL_HEIGHT;
    int y2 = packet / TINYGL_HEIGHT;
    
    tinygl_point_t pos;
    pos.x = TINYGL_HEIGHT - 1 - y1; // lpos y when x = 0
    pos.y = TINYGL_HEIGHT - 1 - y2; // rpos y when x = 0
    return pos;
}

tinygl_point_t convertCharToBall(char packet){
    packet -= 128;
    int x = packet % TINYGL_WIDTH;
    int y = packet / TINYGL_WIDTH;
    
    tinygl_point_t pos;
    pos.x = TINYGL_WIDTH - 1 - x;
    pos.y = TINYGL_HEIGHT - 1 - y;
    return pos;
}

unsigned char convertPaddleToChar(paddle_t paddle_state){
    unsigned char packet = 0;
    unsigned char position = paddle_state.lpos.y + (paddle_state.rpos.y * TINYGL_HEIGHT);
    packet += position;
    return packet;
}

unsigned char convertBallToChar(boing_state_t game_ball){
    unsigned char packet = 128;
    unsigned char position = game_ball.pos.x + (game_ball.pos.y * TINYGL_WIDTH);
    packet += position;
    return packet;
}

char ball_collides(boing_state_t game_ball, paddle_t paddle){
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
    for(int y = min_y; y <= max_y; y++){
        if(game_ball.pos.x == x && game_ball.pos.y == y){
            // Collision!
            return 1;
        }
    }
    
    // No collision
    return 0;
}

boing_state_t boing_update_paddles (boing_state_t state, paddle_t player1, paddle_t player2)
{
    tinygl_point_t hops[] = {{0, 1}, {1, 1}, {1, 0}, {1, -1},
                             {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};

    state.pos.x += hops[state.dir].x;
    state.pos.y += hops[state.dir].y;
    
    if (state.pos.x > TINYGL_WIDTH - 1 || state.pos.x < 0)
    {
        // game score?
    }
    
    if(ball_collides(state, player1) == 1 || ball_collides(state, player2)){
        
        // Bounce from paddle
        boing_dir_t newdir[] = {DIR_N, DIR_NW, DIR_W, DIR_SW,
                                DIR_S, DIR_SE, DIR_E, DIR_NE};
        state.pos.x -= 2 * hops[state.dir].x;
        state.dir = newdir[state.dir];
    }
    //// bounce everywhere for now
    //if (state.pos.x > TINYGL_WIDTH - 1 || state.pos.x < 0)
    //{
        //boing_dir_t newdir[] = {DIR_N, DIR_NW, DIR_W, DIR_SW,
                                //DIR_S, DIR_SE, DIR_E, DIR_NE};
        //state.pos.x -= 2 * hops[state.dir].x;
        //state.dir = newdir[state.dir];
    //}
    
    if (state.pos.y > TINYGL_HEIGHT - 1 || state.pos.y < 0)
    {
        boing_dir_t newdir[] = {DIR_S, DIR_SE, DIR_E, DIR_NE,
                                DIR_N, DIR_NW, DIR_W, DIR_SW};
        state.pos.y -= 2 * hops[state.dir].y;
        state.dir = newdir[state.dir];
    }    

    return state;
}


int main (void)
{
    //int playerNo = 1;
    bool startScreen = true;
    int tick;

    system_init ();
    tinygl_init (LOOP_RATE);

    pacer_init (LOOP_RATE);
    tick = 0;
    boing_state_t ball;
    ball = boing_init (0, 0, DIR_NE);

    //PADDLE -----
    paddle_t paddle;
    paddle_t otherPaddle;
    paddle = init_paddle();
    otherPaddle = init_other_paddle();



    navswitch_init ();


    //ir_serial_init ();
    ir_uart_init ();


    //START SCREEN---------------------

    tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
    tinygl_text ("CLICK TO PLAY ");
    
    button_init();
    
    while (startScreen == true)
    {
        pacer_wait ();

        tinygl_update ();
        navswitch_update ();

        if(button_pressed_p()){
            is_host = 1;
            tinygl_text ("HOST MODE");
        }
        
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            startScreen = false;
            tinygl_clear ();
            //set to player 1
            //set startscreen other player to false
        }
    }

    //MAIN GAME--------------------

    tinygl_draw_line (paddle.lpos, paddle.rpos, 1);
    tinygl_draw_line (otherPaddle.lpos, otherPaddle.rpos, 1);


    while (1)
    {
        pacer_wait ();

        tick++;
        
        if (tick >= 100)
        {

            tick = 0;

            /* Erase previous position.  */
            tinygl_draw_point (ball.pos, 0);

            /* Check for collision; if so reverse direction.  */
            if(is_host == 1){
                ball = boing_update_paddles(ball, paddle, otherPaddle);
                    
                // Send the ball position if host            
                if(is_host == 1){
                    ir_uart_putc(convertBallToChar(ball));
                }
            }
            tinygl_draw_point(ball.pos, 1);
        }



        // PADDLE --------------------------------------------------

        char opp_dir;

        navswitch_update ();

        if (navswitch_push_event_p (NAVSWITCH_NORTH))
        {
            paddle = go_left(paddle);
            //ir_uart_putc ('r');
            
            // Send my paddle position packet
            ir_uart_putc(convertPaddleToChar(paddle));
            
        }

        if (navswitch_push_event_p (NAVSWITCH_SOUTH))
        {
            paddle = go_right(paddle);
            //ir_uart_putc ('l');
            
            // Send my paddle position packet
            ir_uart_putc(convertPaddleToChar(paddle));
        }
        
        while (ir_uart_read_ready_p ()) {
            
            unsigned char packet = ir_uart_getc();
            if(is_host == 0 && packet >= 128){
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

           /*opp_dir = ir_uart_getc ();
           if (opp_dir == 'l')
           {
               otherPaddle = go_left(otherPaddle);
           } else if (opp_dir == 'r') {
               otherPaddle = go_right(otherPaddle);
           }*/

           // Process the received byte.
        }

        tinygl_update();



    }
}
