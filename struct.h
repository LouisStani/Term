#include <stdint.h>

#define SIZE_X 80
#define SIZE_Y 24
#define BG_COLOR_BASE 0
#define COLOR_BASE 0
#define STYLE_BASE 0

struct ele
{
    uint32_t c;
    uint32_t col;
    uint32_t bcol;
    uint8_t styles;
};
struct term
{
    int curs_x;
    int curs_y;
    uint8_t styl_pen;
    uint32_t col_pen;
    uint32_t bcol_pen;
    struct ele cells[SIZE_Y][SIZE_X];
};
