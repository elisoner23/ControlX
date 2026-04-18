# ControlX: Sistema de Seguridad IoT con ESP8266 🔐

Este proyecto es una solución de control de acceso basada en Internet de las Cosas (IoT). Utiliza tecnología RFID para la validación de identidad y se integra con la API de Telegram para enviar notificaciones de seguridad en tiempo real.

## 🚀 Funcionalidades
- **Validación RFID:** Lectura y comparación de UIDs con base de datos local.
- **Interfaz Visual:** Pantalla LCD 16x2 que indica el estado del sistema (I2C).
- **Notificaciones Remotas:** Alertas instantáneas a Telegram sobre accesos permitidos y denegados.
- **Seguridad:** Uso de conexiones seguras (HTTPS) para la comunicación con el Bot de Telegram.

## 🛠️ Hardware Utilizado
- **Controlador:** ESP8266 (NodeMCU).
- **Lector:** MFRC522 RFID.
- **Visualización:** LCD 16x2 con módulo I2C.
- **Actuador:** Relé de 5V para cerradura electromagnética.

## 📌 Configuración de Pines
| Componente | Pin ESP8266 |
| :--- | :--- |
| **RFID SDA** | D4 |
| **RFID RST** | D3 |
| **LCD SDA** | D2 |
| **LCD SCL** | D1 |

## 📁 Estructura del Proyecto
- `/Herramientas`: Contiene el escáner de tarjetas para obtener los UIDs.
- `/Sistema_Principal`: Código fuente del sistema de acceso completo.

## ⚙️ Instalación
1. Clona el repositorio.
2. Instala las librerías `MFRC522`, `hd44780` y `ESP8266WiFi` en tu IDE de Arduino.
3. Configura tus credenciales de red y tokens de Telegram en las constantes del código.

---
**Desarrollado por:** Erick García
