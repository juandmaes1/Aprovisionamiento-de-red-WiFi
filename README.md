# Aprovisonamiento de red Wifi Con Esp32

## Descripción del Proyecto
Configuración Dinámica de WiFi en ESP32 con Portal Web Local
Descripción del Proyecto

Este proyecto demuestra cómo un ESP32 permite configurar la red WiFi sin reprogramar el dispositivo, usando una interfaz web local servida en modo AP y persistiendo credenciales en memoria no volátil (EEPROM/NVS). El firmware conmuta entre AP/STA según la disponibilidad de credenciales y expone endpoints HTTP para estado, configuración y restablecimiento.

En primer arranque (o sin credenciales), el ESP32 inicia en modo AP, publica un formulario para ingresar SSID y password y guarda en memoria no volátil.

Tras guardar, reinicia y se conecta en modo STA; si hay error/timeout, retorna a AP para reconfiguración.

Existen endpoints REST: /status, /config y /reset, además de un botón físico (GPIO) para borrar credenciales.

## Estructura del Proyecto

```plaintext
ESP32-WiFi-Dinamico/
├── README.md                                   # Documentación del proyecto
├── src
│   └── main.ino                                # Firmware (Arduino)
├── data/                                       # (Opcional) Archivos para SPIFFS/LittleFS
├── docs
│   ├── postman_collection.json                 # Colección Postman para probar endpoints
│   ├── IA_conversations.pdf                    # Anexos con conversaciones de IA
│   └── esquematico.pdf                         # Esquemático eléctrico (botón reset y conexiones)
└── diagrams
    ├── uml_sequence_config.md                  # Diagrama de secuencia: configuración inicial
    ├── uml_sequence_reset.md                   # Diagrama de secuencia: reset y reconfiguración
    ├── state_machine.md                        # Diagrama de estados (FSM)
    ├── components.md                           # Diagrama de componentes (arquitectura lógica)
    └── activity_config.md                      # Diagrama de actividad: flujo de configuración web

```

---

## Dependencias Principales

En el entorno Arduino/PlatformIO se requieren las siguientes librerías y plataformas (referencia conceptual en formato XML para mantener la sintaxis):

```xml
<dependency>
  <name>ESP32 Arduino Core</name>
  <source>Board Manager / PlatformIO platform = espressif32</source>
</dependency>
<dependency>
  <name>WiFi</name>
  <artifactId>WiFi.h (incluida en el Core ESP32)</artifactId>
</dependency>
<dependency>
  <name>WebServer</name>
  <artifactId>WebServer.h (incluida en el Core ESP32)</artifactId>
</dependency>
<dependency>
  <name>EEPROM</name>
  <artifactId>EEPROM.h (incluida en el Core ESP32)</artifactId>
</dependency>
<dependency>
  <name>Preferences (alternativa)</name>
  <artifactId>Preferences.h (NVS, opcional)</artifactId>
</dependency>

```

---

## Configuración de Red

Configuración del Dispositivo y Red

Parámetros por defecto del firmware (referenciados conceptualmente con sintaxis de properties):

```properties
# Punto de acceso para configuración inicial
ap.ssid=ESP32_Config
ap.password=12345678
ap.ip=192.168.4.1

# Tiempo máximo de espera para conexión STA (ms)
sta.connect.timeout=15000

# GPIO para reset de credenciales (mantener ~5 s)
reset.gpio=0
reset.hold.ms=5000

# Persistencia
storage.mode=EEPROM   # opciones: EEPROM | NVS | SPIFFS

```
El portal de configuración está disponible en http://192.168.4.1/ cuando el dispositivo está en modo AP.
---

## Endpoints HTTP

- GET /status
Devuelve estado de conexión y SSID actual:
  ```bash
 # En modo AP
curl http://192.168.4.1/status

# En modo STA (usar IP asignada por el router)
curl http://<ip_router_asignada>/status

  ```
  Responde con el nombre y cantidad del pedido o error si el Circuit Breaker está abierto.
  --> Para terminos practicos y de simulación el Circuit Breaker toma como un error la recepcion de una 'laptop' como tipo de producto.

- **GET /inventario**  
  Obtiene el inventario actualizado por el Consumidor:
  ```bash
  curl http://localhost:8080/inventario
  ```

Se puede acceder a las interfaces (UI's) en:
- http://localhost:8080/test.html
- http://localhost:8080/inventario.html

---

## Diagrama de Arquitectura

![Arquitectura RabbitMQ Microservicios](architecturarabbitmq.png)

---

## Explicación de la Implementación

- **`SpringBootRabbitMqApplication`**  
  Punto de entrada de Spring Boot.

- **`PublisherConfig`**  
  Define la `Queue` usando la propiedad `sacavix.queue.name`.

- **`Publisher`**  
  Componente que publica mensajes en la cola con `RabbitTemplate`.

- **`DummyController`**  
  Expone el endpoint **POST /test** que crea un `Pedido` y lo envía.

- **`DummyService`**  
  Encapsula la lógica de envío al `Publisher`.

- **`Pedido`**  
  DTO serializable con `id`, `producto`, `cantidad`, `precioTotal` y `fecha`.

- **`Consumer`**  
  Escucha la cola con `@RabbitListener`, procesa cada `Pedido` y actualiza un inventario concurrente.

- **`InventarioController`**  
  Expone el endpoint **GET /inventario** para consultar el inventario.

## Contribuciones
Este proyecto fue desarrollado por:
- Nicolas Rodrigues
- Juan Diego Martinez
- Camilo Otalora
