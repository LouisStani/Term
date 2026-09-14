#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

#define MAX_PARAMS 8

enum event_type
{
    EVENT_PRINT,
    EVENT_CSI,
};

struct event
{
    enum event_type type;
    uint32_t c;
    int params[MAX_PARAMS];
    int nb_params;
    char final;
};

enum parser_state
{
    STATE_GROUND,
    STATE_ESCAPE,
    STATE_CSI_PARAM,
};

struct parser
{
    enum parser_state state;
    int params[MAX_PARAMS];
    int nb_params;
    int cur_param;
};

typedef void (*on_event_fn)(void *c_ctx, struct event *c_ev);

void parser_init(struct parser *c_parser);
void parse_byte(struct parser *c_parser, uint8_t c_byte, on_event_fn c_on_event, void *c_ctx);

#endif
