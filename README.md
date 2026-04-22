# Sistema de Monitoreo y Control Distribuido

## Descripción

Este repositorio contiene el firmware de un sistema embebido distribuido orientado a monitoreo ambiental, detección de gases y control de actuadores en entornos industriales.

La arquitectura del sistema está basada en la separación de responsabilidades entre dos unidades de procesamiento:

- LPC55S16 (ARM Cortex-M33): encargado de las tareas críticas en tiempo real.
- ESP32-S3: responsable de la conectividad inalámbrica y comunicación con servicios externos.

Ambos dispositivos se comunican mediante una interfaz UART dedicada.

---

## Arquitectura del Firmware

El código está organizado en dos dominios principales:

### 1. Control en Tiempo Real (LPC55S16)

Incluye módulos relacionados con:

- Adquisición de sensores (temperatura y gases)
- Procesamiento de señales
- Lógica de control y seguridad
- Gestión de entradas digitales (botones de alarma)
- Control de salidas mediante relés
- Manejo de buses de comunicación:
  - I2C (múltiples buses para sensores distribuidos)
  - CAN (integración industrial)
  - UART (comunicación interna y externa)
- Interfaz de usuario (LCD)

### 2. Conectividad (ESP32-S3)

Incluye funcionalidades de:

- Comunicación WiFi y Bluetooth Low Energy (BLE)
- Implementación de protocolos de mensajería (ej. MQTT)
- Enlace con servicios remotos de monitoreo
- Indicadores de estado de conectividad

---

## Funcionalidades Principales

- Monitoreo de temperatura:
  - Sensores basados en MCP9601 distribuidos en múltiples zonas
  - Comunicación mediante buses I2C independientes

- Detección de gases:
  - Sensores para monóxido de carbono (CO) y metano (CH4)

- Gestión de alarmas:
  - Entradas aisladas para activación manual
  - Procesamiento de eventos críticos

- Control de actuadores:
  - Relés para:
    - Alarmas sonoras
    - Corte de suministro de gas
    - Desconexión eléctrica
    - Sistemas de extinción por zona
    - Ventilación de ductos

- Aislamiento galvánico:
  - Protección en entradas y salidas para entornos industriales

- Interfaz de usuario:
  - Visualización de estados y alarmas mediante LCD

- Conectividad remota:
  - Integración con plataformas externas mediante MQTT

---

## Estructura del Repositorio (referencial)

```
/lpc55s16/
  ├── drivers/
  ├── sensors/
  ├── control/
  ├── communication/
  └── ui/

/esp32s3/
  ├── wifi/
  ├── ble/
  ├── mqtt/
  └── uart_bridge/

/common/
  ├── protocols/
  └── definitions/
```

---

## Objetivo del Proyecto

El objetivo del firmware es garantizar:

- Operación determinística en tareas críticas
- Alta confiabilidad en detección de eventos peligrosos
- Escalabilidad mediante arquitectura modular
- Integración con sistemas de monitoreo remoto

---

## Notas

Este repositorio está enfocado exclusivamente en el firmware del sistema. La descripción del hardware y del sistema completo se encuentra documentada externamente.
