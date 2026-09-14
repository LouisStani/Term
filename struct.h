#include <stdint.h>

#ifndef STRUCT_H 
#define STRUCT_H
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

void enable_pen_style(struct term *c_term, short style_to_enable);
void disable_pen_style(struct term *c_term, short style_to_disable);
void change_pen_color(struct term *c_term, uint32_t color);
void change_pen_bg_color(struct term *c_term, uint32_t bg_color);
void reset_pen(struct term *c_term);
void clear_cell(struct term *c_term, int line, int col);
void clear_line(struct term *c_term, int line);
void scroll(struct term *c_term);
void back_line(struct term *c_term);
void jump_line(struct term *c_term);
void back(struct term *c_term);
void begi_line(struct term *c_term);
void tab(struct term *c_term);
void p_char(struct term *c_term, uint32_t to_print);
void clear_all(struct term *c_term);
void screen_init(struct term *c_term);
void erase_in_line(struct term *c_term, int mode);
void erase_in_display(struct term *c_term, int mode);
void cursor_move(struct term *c_term, int row, int col);
void cursor_up(struct term *c_term, int n);
void cursor_down(struct term *c_term, int n);
void cursor_forward(struct term *c_term, int n);
void cursor_back(struct term *c_term, int n);

#endif
