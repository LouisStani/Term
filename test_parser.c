#include <stdio.h>
#include "parser.h"

static int g_tests = 0;
static int g_failures = 0;

#define CHECK(cond) do { \
    g_tests += 1; \
    if (!(cond)) { \
        g_failures += 1; \
        printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
    } \
} while (0)

struct recorder
{
    struct event last;
    int count;
};

static void record_event(void *c_ctx, struct event *c_ev)
{
    struct recorder *c_rec = c_ctx;

    c_rec->last = *c_ev;
    c_rec->count += 1;
}

static void test_ground_prints_char(void)
{
    struct parser p;
    struct recorder r;

    parser_init(&p);
    r.count = 0;
    parse_byte(&p, 'A', record_event, &r);
    CHECK(r.count == 1);
    CHECK(r.last.type == EVENT_PRINT);
    CHECK(r.last.c == 'A');
    CHECK(p.state == STATE_GROUND);
}

static void test_escape_produces_no_event(void)
{
    struct parser p;
    struct recorder r;

    parser_init(&p);
    r.count = 0;
    parse_byte(&p, 0x1b, record_event, &r);
    CHECK(r.count == 0);
    CHECK(p.state == STATE_ESCAPE);
}

static void test_escape_unknown_byte_returns_to_ground(void)
{
    struct parser p;
    struct recorder r;

    parser_init(&p);
    r.count = 0;
    parse_byte(&p, 0x1b, record_event, &r);
    parse_byte(&p, 'X', record_event, &r);
    CHECK(r.count == 0);
    CHECK(p.state == STATE_GROUND);
}

static void test_escape_bracket_enters_csi_param(void)
{
    struct parser p;
    struct recorder r;

    parser_init(&p);
    r.count = 0;
    parse_byte(&p, 0x1b, record_event, &r);
    parse_byte(&p, '[', record_event, &r);
    CHECK(r.count == 0);
    CHECK(p.state == STATE_CSI_PARAM);
}

static void feed(struct parser *c_parser, const char *c_str, on_event_fn c_on_event, void *c_ctx)
{
    while (*c_str)
    {
        parse_byte(c_parser, (uint8_t)*c_str, c_on_event, c_ctx);
        c_str += 1;
    }
}

static void test_csi_single_param(void)
{
    struct parser p;
    struct recorder r;

    parser_init(&p);
    r.count = 0;
    feed(&p, "\x1b[10H", record_event, &r);
    CHECK(r.count == 1);
    CHECK(r.last.type == EVENT_CSI);
    CHECK(r.last.final == 'H');
    CHECK(r.last.nb_params == 1);
    CHECK(r.last.params[0] == 10);
    CHECK(p.state == STATE_GROUND);
}

static void test_csi_multiple_params(void)
{
    struct parser p;
    struct recorder r;

    parser_init(&p);
    r.count = 0;
    feed(&p, "\x1b[10;5H", record_event, &r);
    CHECK(r.count == 1);
    CHECK(r.last.nb_params == 2);
    CHECK(r.last.params[0] == 10);
    CHECK(r.last.params[1] == 5);
}

static void test_csi_empty_param_defaults_to_zero(void)
{
    struct parser p;
    struct recorder r;

    parser_init(&p);
    r.count = 0;
    feed(&p, "\x1b[;5H", record_event, &r);
    CHECK(r.last.nb_params == 2);
    CHECK(r.last.params[0] == 0);
    CHECK(r.last.params[1] == 5);
}

static void test_csi_no_param_final_only(void)
{
    struct parser p;
    struct recorder r;

    parser_init(&p);
    r.count = 0;
    feed(&p, "\x1b[H", record_event, &r);
    CHECK(r.last.nb_params == 1);
    CHECK(r.last.params[0] == 0);
}

static void test_csi_overflow_extra_params_dropped(void)
{
    struct parser p;
    struct recorder r;

    parser_init(&p);
    r.count = 0;
    feed(&p, "\x1b[1;2;3;4;5;6;7;8;9H", record_event, &r);
    CHECK(r.last.nb_params == MAX_PARAMS);
    CHECK(r.last.params[0] == 1);
    CHECK(r.last.params[MAX_PARAMS - 1] == 8);
}

int main(void)
{
    test_ground_prints_char();
    test_escape_produces_no_event();
    test_escape_unknown_byte_returns_to_ground();
    test_escape_bracket_enters_csi_param();
    test_csi_single_param();
    test_csi_multiple_params();
    test_csi_empty_param_defaults_to_zero();
    test_csi_no_param_final_only();
    test_csi_overflow_extra_params_dropped();

    if (g_failures == 0)
        printf("OK: %d tests passed\n", g_tests);
    else
        printf("FAILED: %d/%d tests failed\n", g_failures, g_tests);
    return (g_failures != 0);
}
