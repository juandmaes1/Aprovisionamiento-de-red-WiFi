# Aprovisionamiento-de-red-WiFi
Configuración Dinámica de WiFi en ESP32 (Arduino)
Descripción del Proyecto

Este proyecto demuestra cómo un ESP32 permite la configuración dinámica de la red WiFi sin necesidad de reprogramar el dispositivo. Se ofrece una interfaz web local (modo AP) para capturar SSID y contraseña, se persisten las credenciales en memoria no volátil (EEPROM/NVS), y el sistema intenta la reconexión automática en modo STA. Además, se exponen endpoints HTTP para consultar estado, configurar y restablecer, y se incluye un mecanismo físico de reset mediante botón.

En el primer arranque (o si no hay credenciales), el ESP32 inicia como punto de acceso (AP), sirviendo una página para configurar la red.

Tras guardar credenciales, el dispositivo reinicia y hace WiFi.begin en modo STA; ante error/timeout, vuelve a AP para reconfiguración.

Se incluyen endpoints REST (/status, /config, /reset) y una colección Postman para pruebas.

El código está desarrollado en Arduino (C/C++) y sigue buenas prácticas de diseño y documentación.

Estructura del Proyecto
ESP32-WiFi-Dinamico/
├─ README.md                         # Este documento
├─ src/
│  └─ main.ino                       # Código fuente principal (Arduino)
├─ docs/
│  ├─ postman_collection.json        # Colección Postman (opcional)
│  ├─ IA_conversations.pdf           # Anexos con conversaciones de IA (opcional)
│  └─ esquematico.pdf                # Esquemático eléctrico (opcional)
└─ diagrams/
   ├─ uml_sequence_config.md         # [placeholder] Secuencia: configuración inicial
   ├─ uml_sequence_reset.md          # [placeholder] Secuencia: reset y reconfiguración
   ├─ state_machine.md               # [placeholder] Diagrama de estados (FSM)
   ├─ components.md                  # [placeholder] Diagrama de componentes
   └─ activity_config.md             # [placeholder] Actividad: flujo configuración web

Librerías y Dependencias

Arduino Core para ESP32 (Board Manager).

Librerías estándar:

WiFi.h (conectividad STA/AP).

WebServer.h (servidor HTTP embebido).

EEPROM.h (persistencia simple de SSID/Password).

Nota: Puede migrarse a Preferences (NVS) o LittleFS/SPIFFS si se requiere mayor robustez.

Requisitos Funcionales Implementados

Arranque en modo AP si no hay credenciales almacenadas.

Interfaz web local para ingresar SSID/contraseña (formulario en /).

Persistencia de credenciales en EEPROM (adaptable a NVS/SPIFFS).

Reconexión automática en modo STA con manejo de timeout.

Endpoints HTTP para status, config, reset.

Mecanismo de restablecimiento por botón físico (GPIO) o por endpoint.

Documentación técnica y diagramas (marcadores incluidos).

Colección Postman para validación de endpoints.

Instalación y Ejecución

Preparación del entorno

Instalar Arduino IDE 2.x o PlatformIO.

Instalar el ESP32 Arduino Core en el Gestor de Tarjetas.

Seleccionar la placa ESP32 y el puerto correspondiente.

Cargar el firmware

Abrir src/main.ino.

Compilar y subir el sketch al ESP32.

Primer uso (sin credenciales)

El dispositivo expone una red:

SSID: ESP32_Config

Password: 12345678

Conectarse desde el navegador a http://192.168.4.1/ y completar el formulario.

El ESP32 reiniciará y tratará de conectarse a la red configurada en modo STA.

Reconfiguración

Mantener presionado el botón en GPIO0 ~5 segundos para borrar credenciales y volver a modo AP, o

Invocar GET /reset.

Endpoints HTTP

Base URL

Modo AP: http://192.168.4.1

Modo STA: http://<ip_asignada_por_router>

Método	Ruta	Headers	Query/Payload	Respuesta (200)
GET	/status	Content-Type: application/json	—	`{"ssid":"<ssid>","connected":true
POST	/config	application/x-www-form-urlencoded o application/json	ssid, password (en body)	{"status":"Credenciales guardadas, reiniciando..."}
GET	/reset	Content-Type: application/json	—	{"status":"Credenciales borradas"} y luego reinicio

Ejemplos con curl

# Estado (AP o STA)
curl http://192.168.4.1/status

# Configuración (form URL-encoded)
curl -X POST -d "ssid=MiRed&password=Secreta123" http://192.168.4.1/config

# Configuración (JSON)
curl -X POST -H "Content-Type: application/json" \
     -d '{"ssid":"MiRed","password":"Secreta123"}' \
     http://192.168.4.1/config

# Reset remoto
curl http://192.168.4.1/reset

Validación Funcional

Arranque sin credenciales: aparición del AP ESP32_Config, acceso a portal, guardado en EEPROM, reinicio.

Conexión STA correcta: obtención de IP por DHCP, respuesta positiva en /status.

Error/timeout de conexión: retorno automático a modo AP para reconfigurar.

Reset: botón físico o GET /reset retorna a modo AP y borra credenciales.

Buenas Prácticas de Ingeniería

Código comentado y separación de responsabilidades: WiFi (STA/AP), HTTP, persistencia, GPIO.

Manejo de timeouts en conexión y fallback a AP.

Documentación de endpoints, estructura y flujo de uso.

Diagrama de estados (FSM) que guía el comportamiento del gestor de WiFi.

Seguridad y Consideraciones

La contraseña se almacena en texto claro en EEPROM por simplicidad académica. Para producción:

Usar NVS/Preferences y/o cifrado/obfuscación en reposo.

Limitar el tiempo de exposición del AP y fortificar la contraseña del AP.

Implementar portal cautivo real si se requiere redirección automática.

Considerar OTA y logs remotos para mantenimiento.

Diagramas (placeholders)

Secuencia: configuración inicial (sin credenciales)
diagrams/uml_sequence_config.md ← insertar aquí el diagrama

Secuencia: reset y reconfiguración
diagrams/uml_sequence_reset.md ← insertar aquí el diagrama

Diagrama de estados (FSM)
diagrams/state_machine.md ← insertar aquí el diagrama

Diagrama de componentes
diagrams/components.md ← insertar aquí el diagrama

Diagrama de actividad: flujo de configuración web
diagrams/activity_config.md ← insertar aquí el diagrama

Colección Postman

Archivo sugerido: docs/postman_collection.json

Variables recomendadas:

base_url_ap = http://192.168.4.1

base_url_sta = http://<ip_asignada_por_router>

Requests:

GET {{base_url}}/status

POST {{base_url}}/config (body x-www-form-urlencoded o raw JSON)

GET {{base_url}}/reset
