#include "interface/interface.h"

/* ------------------------------------------------------------------ */
/*  Internal type — private to this file                              */
/* ------------------------------------------------------------------ */

typedef struct
{
    void    (*init)          (void);
    void    (*send)          (uint8_t *buf, uint32_t len);
    uint8_t (*receive)       (uint8_t *buf, uint32_t len);
    uint8_t (*data_available)(void);
    void    (*deinit)        (void);
}comm_instance_t;

/* ------------------------------------------------------------------ */
/*  Forward declarations — protocol implementations are internal       */
/* ------------------------------------------------------------------ */

extern void    uart2_protocol_init          (void);
extern void    uart2_protocol_send          (uint8_t *data, uint32_t len);
extern uint8_t uart2_protocol_receive       (uint8_t *buffer, uint32_t len);
extern uint8_t uart2_protocol_data_available(void);

extern void    i2c1_protocol_init   (void);
extern void    i2c1_protocol_send   (uint8_t *data, uint32_t len);
extern uint8_t i2c1_protocol_receive(uint8_t *buffer, uint32_t len);

/* ------------------------------------------------------------------ */
/*  Dispatch table                                                     */
/* ------------------------------------------------------------------ */

static const comm_instance_t s_comm_table[] = {
    [0] = {
        .init           = uart2_protocol_init,
        .send           = uart2_protocol_send,
        .receive        = uart2_protocol_receive,
        .data_available = uart2_protocol_data_available,
        .deinit         = NULL,
    },
    [1] = {
        .init           = i2c1_protocol_init,
        .send           = i2c1_protocol_send,
        .receive        = i2c1_protocol_receive,
        .data_available = NULL,
        .deinit         = NULL,
    },
};

#define COMM_COUNT ((uint8_t)(sizeof(s_comm_table) / sizeof(s_comm_table[0])))

static const comm_instance_t *comm_dispatch(uint8_t comm_id)
{
    if (comm_id >= COMM_COUNT) return NULL;
    return &s_comm_table[comm_id];
}

/* ================================================================== */
/*  Public functions declared in interface.h                          */
/* ================================================================== */

void comm_init(uint8_t comm_id)
{
    const comm_instance_t *c = comm_dispatch(comm_id);
    if (c && c->init) c->init();
}

void comm_send(uint8_t comm_id, uint8_t *buf, uint32_t len)
{
    const comm_instance_t *c = comm_dispatch(comm_id);
    if (c && c->send) c->send(buf, len);
}

uint8_t comm_receive(uint8_t comm_id, uint8_t *buf, uint32_t len)
{
    const comm_instance_t *c = comm_dispatch(comm_id);
    if (c && c->receive) return c->receive(buf, len);
    return 0u;
}

uint8_t comm_data_available(uint8_t comm_id)
{
    const comm_instance_t *c = comm_dispatch(comm_id);
    if (c && c->data_available) return c->data_available();
    return 0u;
}

void comm_deinit(uint8_t comm_id)
{
    const comm_instance_t *c = comm_dispatch(comm_id);
    if (c && c->deinit) c->deinit();
}