# Comunicación entre Microservicios usando RabbitMQ como Broker de Mensajes

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

En **pom.xml** se incluyen:

```xml
<dependency>
  <groupId>org.springframework.boot</groupId>
  <artifactId>spring-boot-starter-amqp</artifactId>
</dependency>
<dependency>
  <groupId>org.springframework.boot</groupId>
  <artifactId>spring-boot-starter-web</artifactId>
</dependency>
<dependency>
  <groupId>io.github.resilience4j</groupId>
  <artifactId>resilience4j-spring-boot2</artifactId>
</dependency>
<dependency>
  <groupId>org.projectlombok</groupId>
  <artifactId>lombok</artifactId>
  <optional>true</optional>
</dependency>
<dependency>
  <groupId>org.springframework.boot</groupId>
  <artifactId>spring-boot-starter-test</artifactId>
  <scope>test</scope>
</dependency>
```

---

## Configuración de RabbitMQ

Por defecto, la aplicación usa las siguientes propiedades (en `application.properties`):

```properties
spring.rabbitmq.host=localhost
spring.rabbitmq.port=5672
spring.rabbitmq.username=guest
spring.rabbitmq.password=guest

sacavix.queue.name=cola1
```
La consola de administración de RabbitMQ estará disponible en http://localhost:15672 (guest/guest).

---

## Endpoints HTTP

- **POST /test**  
  Envía un `Pedido` al microservicio Productor:
  ```bash
  curl -X POST http://localhost:8080/test \
       -H "Content-Type: application/json" \
       -d '{"producto":"Tablet","cantidad":2}'
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
