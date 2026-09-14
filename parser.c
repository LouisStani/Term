#include "parser.h"

void parser_init(struct parser *c_parser)
{
    c_parser->state = STATE_GROUND;
    c_parser->nb_params = 0;
    c_parser->cur_param = 0;
}

static void handle_ground(struct parser *c_parser, uint8_t c_byte, on_event_fn c_on_event, void *c_ctx)
{
    struct event c_ev;

    if (c_byte == 0x1b)
    {
        c_parser->state = STATE_ESCAPE;
        return;
    }
    c_ev.type = EVENT_PRINT;
    c_ev.c = c_byte;
    c_on_event(c_ctx, &c_ev);
}

static void handle_escape(struct parser *c_parser, uint8_t c_byte)
{
    if (c_byte == '[')
    {
        c_parser->state = STATE_CSI_PARAM;
        c_parser->nb_params = 0;
        c_parser->cur_param = 0;
        return;
    }
    c_parser->state = STATE_GROUND;
}

static void push_param(struct parser *c_parser)
{
    if (c_parser->nb_params < MAX_PARAMS)
    {
        c_parser->params[c_parser->nb_params] = c_parser->cur_param;
        c_parser->nb_params += 1;
    }
    c_parser->cur_param = 0;
}

static void handle_csi_param(struct parser *c_parser, uint8_t c_byte, on_event_fn c_on_event, void *c_ctx)
{
    struct event c_ev;
    int c_i;

    if (c_byte >= '0' && c_byte <= '9')
    {
        c_parser->cur_param = c_parser->cur_param * 10 + (c_byte - '0');
        return;
    }
    if (c_byte == ';')
    {
        push_param(c_parser);
        return;
    }
    push_param(c_parser);
    c_ev.type = EVENT_CSI;
    c_ev.final = c_byte;
    c_ev.nb_params = c_parser->nb_params;
    c_i = 0;
    while (c_i < c_parser->nb_params)
    {
        c_ev.params[c_i] = c_parser->params[c_i];
        c_i += 1;
    }
    c_parser->state = STATE_GROUND;
    c_parser->nb_params = 0;
    c_parser->cur_param = 0;
    c_on_event(c_ctx, &c_ev);
}

void parse_byte(struct parser *c_parser, uint8_t c_byte, on_event_fn c_on_event, void *c_ctx)
{
    if (c_parser->state == STATE_GROUND)
        handle_ground(c_parser, c_byte, c_on_event, c_ctx);
    else if (c_parser->state == STATE_ESCAPE)
        handle_escape(c_parser, c_byte);
    else if (c_parser->state == STATE_CSI_PARAM)
        handle_csi_param(c_parser, c_byte, c_on_event, c_ctx);
}
