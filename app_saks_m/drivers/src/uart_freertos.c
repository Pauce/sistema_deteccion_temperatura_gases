/**
 * @file uart_drv.c
 * @brief Implementación del driver UART con ring buffer para LPC55S16.
 *
 * Principios de diseño:
 *  - La ISR (uart_drv_isr_handler) solo escribe en el ring buffer y
 *    llama a xTaskNotifyFromISR si se supera el threshold. Es mínima.
 *  - Las operaciones de lectura (uart_drv_read) consumen datos del buffer
 *    avanzando el tail con sección crítica mínima.
 *  - count es el único campo que tanto la ISR como la tarea modifican,
 *    por eso se protege con taskENTER_CRITICAL / portDISABLE_INTERRUPTS.
 *  - No hay malloc, no hay DMA, no hay dependencia de fsl_usart_freertos.h.
 */
/*
 * uart_freertos.c
 *
 *  Created on: 16/04/2026
 *      Author: plibreros
 */
#include "uart_freertos.h"
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "fsl_usart.h"

/* -------------------------------------------------------------------------
 * Verificación en tiempo de compilación
 * ----------------------------------------------------------------------- */

_Static_assert((UART_DRV_BUF_SIZE & (UART_DRV_BUF_SIZE - 1U)) == 0U,
               "UART_DRV_BUF_SIZE debe ser potencia de 2");

#define BUF_MASK    (UART_DRV_BUF_SIZE - 1U)

/* -------------------------------------------------------------------------
 * Helpers internos del ring buffer
 * ----------------------------------------------------------------------- */

static inline bool rb_push_from_isr(uart_drv_handle_t *h, uint8_t byte)
{
    if (h->count >= UART_DRV_BUF_SIZE) {
        h->rx_overruns++;
        return false;
    }
    h->buf[h->head] = byte;
    h->head = (h->head + 1U) & BUF_MASK;
    h->count++;
    return true;
}

static inline int32_t rb_pop(uart_drv_handle_t *h)
{
    if (h->count == 0U) {
        return -1;
    }
    uint8_t byte = h->buf[h->tail];
    h->tail = (h->tail + 1U) & BUF_MASK;
    taskENTER_CRITICAL();
    h->count--;
    taskEXIT_CRITICAL();
    return (int32_t)byte;
}

/* -------------------------------------------------------------------------
 * API pública
 * ----------------------------------------------------------------------- */

uart_drv_status_t uart_drv_init(uart_drv_handle_t *h,
                                const uart_drv_config_t *config)
{
    if (h == NULL || config == NULL || config->base == NULL) {
        return kUartDrv_ErrParam;
    }
    if (config->task == NULL || config->notify_bit == 0U) {
        return kUartDrv_ErrParam;
    }
    if (config->threshold == 0U || config->threshold > UART_DRV_BUF_SIZE) {
        return kUartDrv_ErrParam;
    }

    memset(h, 0, sizeof(*h));

    h->base       = config->base;
    h->task       = config->task;
    h->notify_bit = config->notify_bit;
    h->threshold  = config->threshold;

    usart_config_t usart_cfg;
    USART_GetDefaultConfig(&usart_cfg);
    usart_cfg.baudRate_Bps = config->baud_rate;
    usart_cfg.enableRx     = true;
    usart_cfg.enableTx     = true;

    status_t sdk_status = USART_Init(config->base, &usart_cfg, config->src_clk_hz);
    if (sdk_status != kStatus_Success) {
        return kUartDrv_ErrParam;
    }

    USART_EnableInterrupts(config->base, kUSART_RxLevelInterruptEnable);
    NVIC_SetPriority(config->irq, config->irq_prio);
    EnableIRQ(config->irq);

    h->initialized = true;
    return kUartDrv_Ok;
}

void uart_drv_deinit(uart_drv_handle_t *h)
{
    if (h == NULL || !h->initialized) {
        return;
    }
    USART_DisableInterrupts(h->base,
                            kUSART_RxLevelInterruptEnable |
                            kUSART_TxLevelInterruptEnable);
    USART_Deinit(h->base);
    h->initialized = false;
}

size_t uart_drv_read(uart_drv_handle_t *h, uint8_t *dst, size_t len)
{
    if (h == NULL || !h->initialized || dst == NULL || len == 0U) {
        return (int32_t)kUartDrv_ErrParam;
    }

    size_t read = 0U;
    while (read < len) {
        int32_t b = rb_pop(h);
        if (b < 0) { break; }
        dst[read++] = (uint8_t)b;
    }
    return (int32_t)read;
}

size_t uart_drv_available(const uart_drv_handle_t *h)
{
    if (h == NULL || !h->initialized) { return 0U; }
    return h->count;
}

void uart_drv_flush(uart_drv_handle_t *h)
{
    if (h == NULL || !h->initialized) { return; }
    taskENTER_CRITICAL();
    h->head  = 0U;
    h->tail  = 0U;
    h->count = 0U;
    taskEXIT_CRITICAL();
}

uart_drv_status_t uart_drv_write(uart_drv_handle_t *h,
                                 const uint8_t *src, size_t len)
{
    if (h == NULL || !h->initialized || src == NULL || len == 0U) {
        return kUartDrv_ErrParam;
    }
    USART_WriteBlocking(h->base, src, len);
    return kUartDrv_Ok;
}

/* -------------------------------------------------------------------------
 * ISR handler compartido
 * ----------------------------------------------------------------------- */

void uart_drv_isr_handler(uart_drv_handle_t *h)
{
    BaseType_t higher_prio_woken = pdFALSE;

    uint32_t status = USART_GetStatusFlags(h->base);

    while (status & kUSART_RxFifoNotEmptyFlag) {
        uint8_t byte = (uint8_t)USART_ReadByte(h->base);
        h->rx_total++;
        rb_push_from_isr(h, byte);

        /*
         * Notificar usando el bit específico de esta instancia.
         * eSetBits permite que múltiples UARTs notifiquen a la misma tarea
         * sin pisarse: cada uno setea su propio bit y la tarea los lee todos.
         */
        if (h->count == h->threshold) {
            if (h->task != NULL) {
                xTaskNotifyFromISR(h->task,
                                   h->notify_bit,
                                   eSetBits,
                                   &higher_prio_woken);
            }
        }

        status = USART_GetStatusFlags(h->base);
    }

    if (status & kUSART_RxError) {
        USART_ClearStatusFlags(h->base, kUSART_RxError);
    }

    portYIELD_FROM_ISR(higher_prio_woken);
}
