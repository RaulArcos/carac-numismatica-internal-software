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
