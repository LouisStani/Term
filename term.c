#include <stdint.h>
#include <stdio.h>
#include "struct.h"
// PARSEUR
// FIN PARSEUR
// GRID

struct term grid;
struct term *p_grid = &grid;

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

void clear_line(struct term *c_term, int line)
{
    int c_col = 0;
    while (c_col < SIZE_X)
    {
        struct ele* l_clear = &c_term->cells[line][c_col];
        l_clear->c = 0;
        l_clear->bcol = BG_COLOR_BASE;
        l_clear->col = COLOR_BASE;
        l_clear->styles = STYLE_BASE;
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
    {
        c_term->curs_y -= 1;
    }
}

void jump_line(struct term* c_term)  // \n
{
    if (c_term->curs_y +1 >= SIZE_Y)
    {
        scroll(c_term);
        clear_line(c_term, SIZE_Y - 1);
    }
    else
    {
        c_term->curs_y += 1;
    }
}

void back(struct term* c_term) // \b
{
    if (c_term->curs_x > 0)
    {
        c_term->curs_x -= 1;
    }
}

void begi_line(struct term* c_term)
{
    c_term->curs_x = 0;
}

void tab(struct term* c_term)
{
    int target = c_term->curs_x - c_term->curs_x % 8 + 8;
    if (target < SIZE_X)
    {
        c_term->curs_x = target;
    }
    else
    {
        c_term->curs_x = SIZE_X - 1;
    }
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
    c_term->bcol_pen = BG_COLOR_BASE;
    c_term->col_pen = COLOR_BASE;
    c_term->styl_pen = STYLE_BASE;
    c_term->curs_x = 0;
    c_term->curs_y = 0;
}

int main()
{
    printf("TEST GRID\n");
    printf("%d\n",p_grid->curs_x);
    printf("%d\n",p_grid->curs_y);
    p_char(p_grid, 100);
    tab(p_grid);
    p_char(p_grid, 7982);
    int y = 0;
    while(y < SIZE_Y)
    {
        int x = 0;
        while (x < SIZE_X)
        {
            printf("%d|", p_grid->cells[y][x].c);
            x ++;
        }
        printf("\n");
        y ++;
    }
    printf("CLEARRRR\n");
    clear_line(p_grid, 0);
    begi_line(p_grid);
    printf("%d\n",p_grid->curs_x);
    printf("%d\n",p_grid->curs_y);
    p_char(p_grid, 123);
    y = 0;
    while(y < SIZE_Y)
    {
        int x = 0;
        while (x < SIZE_X)
        {
            printf("%d|", p_grid->cells[y][x].c);
            x ++;
        }
        printf("\n");
        y ++;
    }
}
