#include <stdint.h>
#include <stdio.h>
#include "struct.h"

void enable_pen_style(struct term *c_term, short style_to_enable){
    c_term->styl_pen = c_term->styl_pen | (1 << style_to_enable);
}

void disable_pen_style(struct term *c_term, short style_to_disable)
{
    c_term->styl_pen = c_term->styl_pen & ~(1 << style_to_disable);
}

void change_pen_color(struct term *c_term, uint32_t color)
{
    c_term->col_pen = color;
}

void change_pen_bg_color(struct term *c_term, uint32_t bg_color)
{
    c_term->bcol_pen = bg_color;
}

void reset_pen(struct term *c_term)
{
    c_term->col_pen = COLOR_BASE;
    c_term->bcol_pen = BG_COLOR_BASE;
    c_term->styl_pen = STYLE_BASE;
}

void clear_cell(struct term *c_term, int line, int col)
{
    struct ele* l_clear = &c_term->cells[line][col];
    l_clear->c = 0;
    l_clear->bcol = c_term->bcol_pen;
    l_clear->col = c_term->col_pen;
    l_clear->styles = c_term->styl_pen;
}

void clear_line(struct term *c_term, int line)
{
    int c_col = 0;
    while (c_col < SIZE_X)
    {
        clear_cell(c_term, line, c_col);
        c_col+=1;
    }
}

void scroll(struct term* c_term)
{
    int c_row=1;
    while(c_row < SIZE_Y)
    {
        int c_column=0;
        while (c_column < SIZE_X)
        {
            c_term->cells[c_row - 1][c_column] = c_term->cells[c_row][c_column];
            c_column += 1;
        }
        c_row += 1;
    }
}

void back_line(struct term* c_term)
{
    if (c_term->curs_y-1 >= 0)
        c_term->curs_y -= 1;
}

void jump_line(struct term* c_term)  // \n
{
    if (c_term->curs_y +1 >= SIZE_Y)
    {
        scroll(c_term);
        clear_line(c_term, SIZE_Y - 1);
    }
    else
        c_term->curs_y += 1;
}

void back(struct term* c_term) // \b
{
    if (c_term->curs_x > 0)
        c_term->curs_x -= 1;
}

void begi_line(struct term* c_term)
{
    c_term->curs_x = 0;
}

void tab(struct term* c_term)
{
    int target = c_term->curs_x - c_term->curs_x % 8 + 8;
    if (target < SIZE_X)
        c_term->curs_x = target;
    else
        c_term->curs_x = SIZE_X - 1;
}

void p_char(struct term* c_term, uint32_t to_print)
{
    struct ele* c_cell = &c_term->cells[c_term->curs_y][c_term->curs_x];
    c_cell->c = to_print;
    c_cell->styles = c_term->styl_pen;
    c_cell->col = c_term->col_pen;
    c_cell->bcol = c_term->bcol_pen;
    c_term->curs_x += 1;
    if (c_term->curs_x>=SIZE_X)
    {
        begi_line(c_term);
        jump_line(c_term);
    }
}

void clear_all(struct term *c_term)
{
    int y = 0;
    while (y < SIZE_Y) {
        clear_line(c_term, y);
        y+=1;
    }
}

void screen_init(struct term *c_term)
{
    c_term->curs_x = 0;
    c_term->curs_y = 0;
    reset_pen(c_term);
    clear_all(c_term);
}

void erase_in_line(struct term *c_term, int mode)
{
    int c_col;

    if (mode == 2) // Clear line 
    {
        clear_line(c_term, c_term->curs_y);
        return;
    }
    if (mode == 0) // Clear from curs to end 
    {
        c_col = c_term->curs_x;
        while (c_col < SIZE_X)
        {
            clear_cell(c_term, c_term->curs_y, c_col);
            c_col += 1;
        }
    }
    else if (mode == 1) // Clear from start to curs 
    {
        c_col = 0;
        while (c_col <= c_term->curs_x)
        {
            clear_cell(c_term, c_term->curs_y, c_col);
            c_col += 1;
        }
    }
}

void erase_in_display(struct term *c_term, int mode)
{
    int c_row;

    if (mode == 2)
    {
        clear_all(c_term);
        return;
    }
    if (mode == 0)
    {
        erase_in_line(c_term, 0);
        c_row = c_term->curs_y + 1;
        while (c_row < SIZE_Y)
        {
            clear_line(c_term, c_row);
            c_row += 1;
        }
    }
    else if (mode == 1)
    {
        erase_in_line(c_term, 1);
        c_row = 0;
        while (c_row < c_term->curs_y)
        {
            clear_line(c_term, c_row);
            c_row += 1;
        }
    }
}

void cursor_move(struct term *c_term, int row, int col)
{
    if (row < 0)
        row = 0;
    if (row >= SIZE_Y)
        row = SIZE_Y - 1;
    if (col < 0)
        col = 0;
    if (col >= SIZE_X)
        col = SIZE_X - 1;
    c_term->curs_y = row;
    c_term->curs_x = col;
}

void cursor_up(struct term *c_term, int n)
{
    c_term->curs_y -= n;
    if (c_term->curs_y < 0)
        c_term->curs_y = 0;
}

void cursor_down(struct term *c_term, int n)
{
    c_term->curs_y += n;
    if (c_term->curs_y >= SIZE_Y)
        c_term->curs_y = SIZE_Y - 1;
}

void cursor_forward(struct term *c_term, int n)
{
    c_term->curs_x += n;
    if (c_term->curs_x >= SIZE_X)
        c_term->curs_x = SIZE_X - 1;
}

void cursor_back(struct term *c_term, int n)
{
    c_term->curs_x -= n;
    if (c_term->curs_x < 0)
        c_term->curs_x = 0;
}

