#ifndef PADDLE_H
#define PADDLE_H

#include "system.h"
#include "tinygl.h"

typedef struct
{
    tinygl_point_t lpos;
    tinygl_point_t rpos;
} paddle_t;

/**Move the paddle left. */
paddle_t go_left(paddle_t paddle);

/**Move the paddle right. */
paddle_t go_right(paddle_t paddle);

/**Initialise the paddle. */
paddle_t init_paddle(void);

/**Initialise the other player's paddle */
paddle_t init_other_paddle(void);

#endif
