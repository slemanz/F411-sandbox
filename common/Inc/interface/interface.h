#ifndef INC_INTERFACE_H_
#define INC_INTERFACE_H_

#include <stdint.h>
#include <stddef.h>

/************************************************************
*                       TIMEBASE                            *
*************************************************************/

typedef struct {
    uint64_t (*get)(void);
    void (*deinit)(void);
}timebase_interface_t;

timebase_interface_t *timebase_get(void);

/************************************************************
*                       COMM                                *
*************************************************************/

typedef struct
{
    void    (*init)(void);
    void    (*send)(uint8_t *buffer, uint32_t len);
    uint8_t (*receive)(uint8_t *buffer, uint32_t len);
    uint8_t (*data_available)(void);
    void    (*deinit)(void);
}Comm_Interface_t;

Comm_Interface_t *Comm_ProtocolGet(uint8_t type);

/************************************************************
*                       IO                                  *
*************************************************************/

typedef enum
{
    IO_OK              =  0,   /**< Operação concluída com sucesso       */
    IO_ERR_NULL        = -1,   /**< Ponteiro nulo recebido               */
    IO_ERR_INVALID_PIN = -2,   /**< pin_id fora do range configurado     */
    IO_ERR_NOT_INIT    = -3,   /**< Pino não inicializado                */
    IO_ERR_HW_FAULT    = -4,   /**< Falha reportada pelo driver GPIO     */
    IO_ERR_TIMEOUT     = -5,   /**< Timeout numa operação de I/O         */
}io_status_t;

#define IO_PIN_HIGH     ((uint8_t)1u)
#define IO_PIN_LOW      ((uint8_t)0u)

#define IO_MODE_OUTPUT  ((uint8_t)0u)
#define IO_MODE_INPUT   ((uint8_t)1u)

io_status_t IO_init(uint8_t pin_id);
io_status_t IO_write(uint8_t pin_id, uint8_t value);
io_status_t IO_read(uint8_t pin_id, uint8_t *out_value);
io_status_t IO_toggle(uint8_t pin_id);
io_status_t IO_set_mode(uint8_t pin_id, uint8_t mode);
io_status_t IO_deinit(uint8_t pin_id);

/************************************************************
*                       ADC                                 *
*************************************************************/

typedef struct
{
    void     (*init)(void);
    uint16_t (*read)(void);         /**< Returns raw ADC value */
    void     (*deinit)(void);
} ADC_Interface_t;

ADC_Interface_t *ADC_Interface_get(uint8_t channel);

/************************************************************
*                       PWM                                 *
*************************************************************/

typedef struct
{
    void (*init)(void);
    void (*set_duty)(float dutyPercent);    /**< 0.0 – 100.0 */
    void (*deinit)(void);
} PWM_Interface_t;

PWM_Interface_t *PWM_Interface_get(uint8_t instance);

#endif /* INC_INTERFACE_H_ */