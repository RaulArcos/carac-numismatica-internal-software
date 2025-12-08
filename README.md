# CARAC - Sistema de Control para Equipo Numismático

## Descripción

Sistema completo para el estudio y digitalización de monedas históricas, desarrollado en el marco del proyecto **WEB ONLINE DE NUMISMÁTICA. DATOS, ENTORNO Y RECONOCIMIENTO DE MONEDAS – HISPANIA** (Universidad de Cádiz).

El sistema CARAC consta de dos componentes software principales:

### 1. Software de Escritorio (Python/PySide6)
- Interfaz gráfica profesional para control del equipo
- Gestión de iluminación LED adaptable (4 anillos independientes)
- Control de secuencias automáticas de digitalización
- Monitorización de peso en tiempo real
- Protocolo de comunicación JSON bidireccional

### 2. Firmware ESP32 (C++/Arduino)
- Control directo de hardware (LEDs, motores, sensores)
- Sistema de iluminación NeoPixel (16 + 21 LEDs)
- Control de motores DC y servomotores para volteo automático
- Sensor de peso HX711 con calibración
- Comunicación serial a 115200 baud con protocolo JSON

## Documentación Técnica

La **memoria técnica completa** en español está disponible en:

📄 **`memoria_tecnica.tex`**

Este documento LaTeX incluye:
- Introducción y contexto del proyecto
- Análisis completo del sistema (requisitos, actores, casos de uso)
- Diseño e implementación detallada de ambos componentes software
- Protocolo de comunicación bidireccional
- Pruebas realizadas y resultados
- Conclusiones y trabajo futuro
- Bibliografía y anexos

### Compilar la Documentación

```bash
pdflatex memoria_tecnica.tex
pdflatex memoria_tecnica.tex  # Segunda pasada para referencias
```

## Características Principales

- ✅ **Iluminación Adaptable**: 4 anillos LED configurables con perfiles predefinidos
- ✅ **Volteo Automático**: Mecanismo de volteo sincronizado con 2 servomotores
- ✅ **Medición de Peso**: Sensor HX711 con promediado de muestras
- ✅ **Secuencias Automáticas**: Digitalización completa (2 caras) con un solo comando
- ✅ **Interfaz Profesional**: UI nativa Qt con tema institucional UCA
- ✅ **Comunicación Robusta**: Protocolo JSON con heartbeat y acknowledgment
- ✅ **Monitorización en Tiempo Real**: Estado de conexión, heartbeat, eventos del sistema

## Estructura del Proyecto

```
carac-numismatica-internal-software/
├── carac-numismatica-internal-software.ino  # Firmware ESP32 (punto de entrada)
├── include/                                  # Headers del firmware
│   ├── config.h                              # Configuración centralizada
│   ├── communication.h                       # Módulo de comunicación
│   ├── led_controller.h                      # Control de LEDs
│   ├── motor_controller.h                    # Control de motores
│   ├── weight_sensor.h                       # Sensor de peso
│   └── system_controller.h                   # Controlador principal
├── src/                                      # Implementaciones
│   ├── communication.cpp
│   ├── led_controller.cpp
│   ├── motor_controller.cpp
│   ├── weight_sensor.cpp
│   └── system_controller.cpp
├── memoria_tecnica.tex                       # Documentación técnica completa
└── README.md                                 # Este archivo
```

## Tecnologías Utilizadas

### Firmware ESP32
- **Plataforma**: ESP32 (Arduino Framework)
- **Lenguaje**: C++
- **Bibliotecas**:
  - ArduinoJson 6.x (protocolo de comunicación)
  - Adafruit NeoPixel 1.11.x (control LED WS2812B)
  - ESP32Servo 0.13.x (control de servomotores)
  - HX711 0.7.x (sensor de peso)

### Software de Escritorio
- **Lenguaje**: Python 3.11+
- **Framework UI**: PySide6 (Qt for Python)
- **Comunicación**: PySerial 3.5
- **Validación**: Pydantic 2.5
- **Logging**: Loguru 0.7

## Desarrollo

### Configuración del Entorno ESP32

1. Instalar Arduino IDE 2.x
2. Añadir soporte para ESP32 en Board Manager
3. Instalar bibliotecas desde Library Manager (ver lista arriba)
4. Configurar board como "ESP32 Dev Module" a 115200 baud
5. Compilar y subir firmware al ESP32

### Configuración Hardware

Conexiones GPIO del ESP32:

| GPIO | Componente | Función |
|------|------------|---------|
| 2    | LED Test | LED de prueba |
| 4    | HX711 DOUT | Datos sensor de peso |
| 5    | Limit Switch 2 | Final de carrera adelante |
| 13   | LED Strip | Control NeoPixel (16 LEDs) |
| 16   | HX711 SCK | Clock sensor de peso |
| 17   | Motor A2 | Control motor DC |
| 18   | Servo Left | Servo izquierdo |
| 19   | Servo Right | Servo derecho |
| 21   | Limit Switch 1 | Final de carrera atrás |
| 22   | Motor A1 | Control motor DC |
| 26   | Backlight | Control backlight (21 LEDs) |

## Protocolo de Comunicación

El sistema utiliza comunicación serial bidireccional con mensajes JSON:

```json
{
  "type": "lighting_set",
  "payload": {
    "sections": {
      "ring_1": 200,
      "ring_2": 180,
      "ring_3": 150,
      "ring_4": 100
    }
  }
}
```

Ver documentación completa en `memoria_tecnica.tex` para especificación detallada del protocolo.

## Autores

**Equipo CARAC**  
Escuela Superior de Ingeniería  
Universidad de Cádiz

**Investigadora Principal:**  
Dra. Elena Moreno Pulido

## Licencia

MIT License - Copyright (c) 2025 Universidad de Cádiz

## Citar este Trabajo

```
CARAC Development Team. (2025). CARAC: Sistema de Control para Equipo Numismático 
(Version 1.0.0) [Software]. Universidad de Cádiz.
```

## Contacto

Para más información sobre el proyecto HISPANIA o el sistema CARAC, contactar con la Escuela Superior de Ingeniería de la Universidad de Cádiz.
