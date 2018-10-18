/**
 @file ball.c
 @authors Sean Plane spl48, John Kim jki80
 @date 18/10/2018
 **/

 #include "ball.h"
 #include "boing.h"
 #include "paddle.h"
 #include "tinygl.h"

int update_ball_speed(int ball_speed)
{
    if (ball_speed > 1) {
        ball_speed--;
    }
    return ball_speed;
}

boing_state_t rebound_off_paddle(boing_state_t ball, tinygl_point_t hops[])
{
    boing_dir_t newdir[] = {DIR_N, DIR_NW, DIR_W, DIR_SW,
                                DIR_S, DIR_SE, DIR_E, DIR_NE};
    ball.pos.x -= 2 * hops[ball.dir].x;
    ball.dir = newdir[ball.dir];
    return ball;
}

boing_state_t rebound_off_side(boing_state_t ball, tinygl_point_t hops[])
{
    boing_dir_t newdir[] = {DIR_S, DIR_SE, DIR_E, DIR_NE,
                                    DIR_N, DIR_NW, DIR_W, DIR_SW};
    ball.pos.y -= 2 * hops[ball.dir].y;
    ball.dir = newdir[ball.dir];
    return ball;
}

boing_state_t move_ball(boing_state_t ball)
 {
    tinygl_point_t hops[] = {{0, 1}, {1, 1}, {1, 0}, {1, -1},
                             {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};

    ball.pos.x += hops[ball.dir].x;
    ball.pos.y += hops[ball.dir].y;
    return ball;
 }

boing_state_t update_ball (boing_state_t ball, paddle_t player1, paddle_t player2)
{
    tinygl_point_t hops[] = {{0, 1}, {1, 1}, {1, 0}, {1, -1},
                             {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};
    if(ball_collides(ball, player1) == 1 || ball_collides(ball, player2)){
        ball = rebound_off_paddle(ball, hops);
    }

    if (ball.pos.y > TINYGL_HEIGHT - 1 || ball.pos.y < 0)
    {
        ball = rebound_off_side(ball, hops);
    }

    return ball;
}
