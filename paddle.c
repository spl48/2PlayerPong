#include <avr/io.h>
#include "system.h"
#include "tinygl.h"
#include "paddle.h"


paddle_t go_left(paddle_t paddle) {
    if (paddle.lpos.y > 0) {
        tinygl_draw_line (paddle.lpos, paddle.rpos, 0);
        paddle.lpos.y = paddle.lpos.y - 1;
        paddle.rpos.y = paddle.rpos.y - 1;
        tinygl_draw_line (paddle.lpos, paddle.rpos, 1);
    }
    return paddle;
}

paddle_t go_right(paddle_t paddle) {
    if (paddle.rpos.y < TINYGL_HEIGHT-1) {
        tinygl_draw_line (paddle.lpos, paddle.rpos, 0);
        paddle.lpos.y = paddle.lpos.y + 1;
        paddle.rpos.y = paddle.rpos.y + 1;
        tinygl_draw_line (paddle.lpos, paddle.rpos, 1);
    }
    return paddle;
}

paddle_t init_paddle() {
    paddle_t paddle;
    paddle.lpos.x = TINYGL_WIDTH - 1;
    paddle.lpos.y = (TINYGL_HEIGHT / 2) - 1;
    paddle.rpos.x = TINYGL_WIDTH - 1;
    paddle.rpos.y = (TINYGL_HEIGHT / 2) + 1;
    return paddle;
}

paddle_t init_other_paddle() {
    paddle_t paddle;
    paddle.lpos.x = 0;
    paddle.lpos.y = (TINYGL_HEIGHT / 2) - 1;
    paddle.rpos.x = 0;
    paddle.rpos.y = (TINYGL_HEIGHT / 2) + 1;
    return paddle;
}
