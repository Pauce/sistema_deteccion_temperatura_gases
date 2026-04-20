/**
 * @file uart_drv.h
 * @brief Driver UART con ring buffer e integración FreeRTOS para LPC55S16.
 *
 * Diseño:
 *  - Usa fsl_usart.h (interrupciones crudas) en lugar de fsl_usart_freertos.h
 *    para tener control total del threshold de notificación y del consumo de datos.
 *  - Una instancia = una estructura uart_drv_handle_t en RAM estática.
 *  - La ISR escribe en el ring buffer y notifica a la tarea registrada cuando
 *    el contador de bytes supera el threshold configurado.
 *  - La tarea consume datos con uart_drv_read() que libera espacio en el buffer.
 *  - Sin DMA, sin malloc.
 */
/*
 * uart_freertos.h
 *
 *  Created on: 16/04/2026
 *      Author: plibreros
 */

#ifndef DRIVERS_INC_UART_FREERTOS_H_
#define DRIVERS_INC_UART_FREERTOS_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* SDK / FreeRTOS */
#include "fsl_usart.h"
#include "FreeRTOS.h"
#include "task.h"

/* -------------------------------------------------------------------------
 * Configuración en tiempo de compilación
 * ----------------------------------------------------------------------- */

/** Tamaño del ring buffer de cada instancia (debe ser potencia de 2). */
#ifndef UART_DRV_BUF_SIZE
#define UART_DRV_BUF_SIZE   256U
#endif

/* -------------------------------------------------------------------------
 * Bits de notificación predefinidos
 *
 * Una tarea puede atender hasta 32 UARTs simultáneos usando bits distintos.
 * Cada instancia que comparta tarea debe usar un bit diferente.
 * ----------------------------------------------------------------------- */

#define UART_DRV_NOTIFY_BIT(n)    (1UL << (uint32_t)(n))

#define UART_DRV_NOTIFY_BIT_0     UART_DRV_NOTIFY_BIT(0)   /**< 0x00000001 */
#define UART_DRV_NOTIFY_BIT_1     UART_DRV_NOTIFY_BIT(1)   /**< 0x00000002 */
#define UART_DRV_NOTIFY_BIT_2     UART_DRV_NOTIFY_BIT(2)   /**< 0x00000004 */
#define UART_DRV_NOTIFY_BIT_3     UART_DRV_NOTIFY_BIT(3)   /**< 0x00000008 */

/* -------------------------------------------------------------------------
 * Códigos de retorno
 * ----------------------------------------------------------------------- */

typedef enum {
    kUartDrv_Ok              =  0,
    kUartDrv_ErrParam        = -1,  /**< Parámetro inválido.               */
    kUartDrv_ErrFull         = -2,  /**< Ring buffer lleno (overflow).      */
    kUartDrv_ErrEmpty        = -3,  /**< Ring buffer vacío.                 */
} uart_drv_status_t;

/* -------------------------------------------------------------------------
 * Handle de instancia
 * ----------------------------------------------------------------------- */

/**
 * @brief Estado interno de una instancia del driver.
 *
 * No acceder directamente: usar las funciones de la API.
 * Se declara completo (no opaco) para permitir instancias en RAM estática
 * sin malloc.
 */
typedef struct {
    /* Periférico */
    USART_Type         *base;

    /* Ring buffer */
    uint8_t             buf[UART_DRV_BUF_SIZE];
    volatile size_t     head;           /**< Índice de escritura (ISR).    */
    volatile size_t     tail;           /**< Índice de lectura (tarea).    */
    volatile size_t     count;          /**< Bytes disponibles para leer.  */

    /* Notificación FreeRTOS */
    TaskHandle_t        task;           /**< Tarea a notificar.            */
    uint32_t            notify_bit;     /**< Bit usado en la notificación. */
    size_t              threshold;      /**< Bytes mínimos para notificar. */

    /* Estadísticas */
    volatile uint32_t   rx_total;
    volatile uint32_t   rx_overruns;    /**< Bytes perdidos por buf lleno. */

    bool                initialized;
} uart_drv_handle_t;

/* -------------------------------------------------------------------------
 * Parámetros de inicialización
 * ----------------------------------------------------------------------- */

typedef struct {
    USART_Type    *base;
    uint32_t       src_clk_hz;
    uint32_t       baud_rate;
    TaskHandle_t   task;
    /**
     * Bit a setear en la notificación FreeRTOS.
     * Usar UART_DRV_NOTIFY_BIT_x. Debe ser único entre todas las instancias
     * que apunten a la misma tarea.
     */
    uint32_t       notify_bit;
    size_t         threshold;
    IRQn_Type      irq;
    uint8_t        irq_prio;    /**< >= configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY */
} uart_drv_config_t;

/* -------------------------------------------------------------------------
 * API pública
 * ----------------------------------------------------------------------- */

uart_drv_status_t uart_drv_init(uart_drv_handle_t *h,
                                const uart_drv_config_t *config);

void uart_drv_deinit(uart_drv_handle_t *h);

/**
 * @brief Lee y consume bytes del ring buffer.
 * @return Bytes leídos, o negativo si error.
 */
size_t uart_drv_read(uart_drv_handle_t *h, uint8_t *dst, size_t len);

size_t uart_drv_available(const uart_drv_handle_t *h);

void uart_drv_flush(uart_drv_handle_t *h);

uart_drv_status_t uart_drv_write(uart_drv_handle_t *h,
                                 const uint8_t *src, size_t len);

/**
 * @brief ISR compartida. Llamar desde cada USART_IRQHandler:
 * @code
 *   void FLEXCOMM0_IRQHandler(void) { uart_drv_isr_handler(&g_uart0); }
 * @endcode
 */
void uart_drv_isr_handler(uart_drv_handle_t *h);

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_INC_UART_FREERTOS_H_ */
