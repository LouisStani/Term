#include <stdio.h>
#include "struct.h"

static int g_tests = 0;
static int g_failures = 0;

#define CHECK(cond) do { \
    g_tests += 1; \
    if (!(cond)) { \
        g_failures += 1; \
        printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
    } \
} while (0)

static void test_screen_init(void)
{
    struct term t;

    screen_init(&t);
    CHECK(t.curs_x == 0);
    CHECK(t.curs_y == 0);
    CHECK(t.col_pen == COLOR_BASE);
    CHECK(t.bcol_pen == BG_COLOR_BASE);
    CHECK(t.styl_pen == STYLE_BASE);
    CHECK(t.cells[0][0].c == 0);
    CHECK(t.cells[0][0].col == COLOR_BASE);
    CHECK(t.cells[0][0].bcol == BG_COLOR_BASE);
    CHECK(t.cells[0][0].styles == STYLE_BASE);
    CHECK(t.cells[SIZE_Y - 1][SIZE_X - 1].c == 0);
}

static void test_p_char_simple(void)
{
    struct term t;

    screen_init(&t);
    change_pen_color(&t, 42);
    change_pen_bg_color(&t, 7);
    enable_pen_style(&t, 0);
    p_char(&t, 'A');
    CHECK(t.cells[0][0].c == 'A');
    CHECK(t.cells[0][0].col == 42);
    CHECK(t.cells[0][0].bcol == 7);
    CHECK(t.cells[0][0].styles == 1);
    CHECK(t.curs_x == 1);
    CHECK(t.curs_y == 0);
}

static void test_p_char_wrap_line(void)
{
    struct term t;

    screen_init(&t);
    t.curs_y = 5;
    t.curs_x = SIZE_X - 1;
    p_char(&t, 'B');
    CHECK(t.cells[5][SIZE_X - 1].c == 'B');
    CHECK(t.curs_x == 0);
    CHECK(t.curs_y == 6);
}

static void test_p_char_wrap_scroll(void)
{
    struct term t;

    screen_init(&t);
    t.cells[1][0].c = 999;
    t.curs_y = SIZE_Y - 1;
    t.curs_x = SIZE_X - 1;
    p_char(&t, 'Z');
    CHECK(t.cells[SIZE_Y - 2][SIZE_X - 1].c == 'Z');
    CHECK(t.cells[0][0].c == 999);
    CHECK(t.cells[SIZE_Y - 1][0].c == 0);
    CHECK(t.curs_x == 0);
    CHECK(t.curs_y == SIZE_Y - 1);
}

static void test_back_line(void)
{
    struct term t;

    screen_init(&t);
    t.curs_y = 5;
    back_line(&t);
    CHECK(t.curs_y == 4);
    t.curs_y = 0;
    back_line(&t);
    CHECK(t.curs_y == 0);
}

static void test_back(void)
{
    struct term t;

    screen_init(&t);
    t.curs_x = 5;
    back(&t);
    CHECK(t.curs_x == 4);
    t.curs_x = 0;
    back(&t);
    CHECK(t.curs_x == 0);
}

static void test_begi_line(void)
{
    struct term t;

    screen_init(&t);
    t.curs_x = 42;
    begi_line(&t);
    CHECK(t.curs_x == 0);
}

static void test_tab(void)
{
    struct term t;

    screen_init(&t);
    t.curs_x = 3;
    tab(&t);
    CHECK(t.curs_x == 8);
    tab(&t);
    CHECK(t.curs_x == 16);
    t.curs_x = SIZE_X - 4;
    tab(&t);
    CHECK(t.curs_x == SIZE_X - 1);
    tab(&t);
    CHECK(t.curs_x == SIZE_X - 1);
}

static void test_clear_cell_uses_current_pen(void)
{
    struct term t;

    screen_init(&t);
    change_pen_color(&t, 11);
    change_pen_bg_color(&t, 22);
    enable_pen_style(&t, 3);
    clear_cell(&t, 0, 0);
    CHECK(t.cells[0][0].c == 0);
    CHECK(t.cells[0][0].col == 11);
    CHECK(t.cells[0][0].bcol == 22);
    CHECK(t.cells[0][0].styles == (1 << 3));
}

static void test_clear_line_uses_current_pen(void)
{
    struct term t;

    screen_init(&t);
    p_char(&t, 'x');
    change_pen_color(&t, 5);
    change_pen_bg_color(&t, 6);
    clear_line(&t, 0);
    CHECK(t.cells[0][0].c == 0);
    CHECK(t.cells[0][0].col == 5);
    CHECK(t.cells[0][0].bcol == 6);
    CHECK(t.cells[0][SIZE_X - 1].col == 5);
}

static void test_clear_all_does_not_touch_pen_or_cursor(void)
{
    struct term t;

    screen_init(&t);
    t.curs_x = 10;
    t.curs_y = 3;
    change_pen_color(&t, 9);
    change_pen_bg_color(&t, 8);
    enable_pen_style(&t, 1);
    clear_all(&t);
    CHECK(t.curs_x == 10);
    CHECK(t.curs_y == 3);
    CHECK(t.col_pen == 9);
    CHECK(t.bcol_pen == 8);
    CHECK(t.styl_pen == (1 << 1));
    CHECK(t.cells[0][0].c == 0);
    CHECK(t.cells[0][0].col == 9);
    CHECK(t.cells[SIZE_Y - 1][SIZE_X - 1].bcol == 8);
}

static void test_reset_pen(void)
{
    struct term t;

    screen_init(&t);
    change_pen_color(&t, 9);
    change_pen_bg_color(&t, 8);
    enable_pen_style(&t, 1);
    reset_pen(&t);
    CHECK(t.col_pen == COLOR_BASE);
    CHECK(t.bcol_pen == BG_COLOR_BASE);
    CHECK(t.styl_pen == STYLE_BASE);
}

int main(void)
{
    test_screen_init();
    test_p_char_simple();
    test_p_char_wrap_line();
    test_p_char_wrap_scroll();
    test_back_line();
    test_back();
    test_begi_line();
    test_tab();
    test_clear_cell_uses_current_pen();
    test_clear_line_uses_current_pen();
    test_clear_all_does_not_touch_pen_or_cursor();
    test_reset_pen();


    if (g_failures == 0)
        printf("OK: %d tests passed\n", g_tests);
    else
        printf("FAILED: %d/%d tests failed\n", g_failures, g_tests);
    return (g_failures != 0);
}
