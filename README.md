| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP-MQTT sample application
(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example connects to the broker URI selected using `idf.py menuconfig` (using mqtt tcp transport) and as a demonstration subscribes/unsubscribes and send a message on certain topic.
(Please note that the public broker is maintained by the community so may not be always available, for details please see this [disclaimer](https://iot.eclipse.org/getting-started/#sandboxes))

Note: If the URI equals `FROM_STDIN` then the broker address is read from stdin upon application startup (used for testing)

It uses ESP-MQTT library which implements mqtt client to connect to mqtt broker.

## How to use example

### Hardware Required

This example can be executed on any ESP32 board, the only required interface is WiFi and connection to internet.

### Configure the project

* Open the project configuration menu (`idf.py menuconfig`)
* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (3714) event: sta ip: 192.168.0.139, mask: 255.255.255.0, gw: 192.168.0.2
I (3714) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (3964) MQTT_CLIENT: Sending MQTT CONNECT message, type: 1, id: 0000
I (4164) MQTT_EXAMPLE: MQTT_EVENT_CONNECTED
I (4174) MQTT_EXAMPLE: sent publish successful, msg_id=41464
I (4174) MQTT_EXAMPLE: sent subscribe successful, msg_id=17886
I (4174) MQTT_EXAMPLE: sent subscribe successful, msg_id=42970
I (4184) MQTT_EXAMPLE: sent unsubscribe successful, msg_id=50241
I (4314) MQTT_EXAMPLE: MQTT_EVENT_PUBLISHED, msg_id=41464
I (4484) MQTT_EXAMPLE: MQTT_EVENT_SUBSCRIBED, msg_id=17886
I (4484) MQTT_EXAMPLE: sent publish successful, msg_id=0
I (4684) MQTT_EXAMPLE: MQTT_EVENT_SUBSCRIBED, msg_id=42970
I (4684) MQTT_EXAMPLE: sent publish successful, msg_id=0
I (4884) MQTT_CLIENT: deliver_publish, message_length_read=19, message_length=19
I (4884) MQTT_EXAMPLE: MQTT_EVENT_DATA
TOPIC=/topic/qos0
DATA=data
I (5194) MQTT_CLIENT: deliver_publish, message_length_read=19, message_length=19
I (5194) MQTT_EXAMPLE: MQTT_EVENT_DATA
TOPIC=/topic/qos0
DATA=data
```
# CREAR LIBRERIAS AZURE EN ESP_IDF

Para crear las librerias como componentes externos de azure inicialmente se dispone la creacion de un nuevo directorio o carpeta dentro del proyecto, denominada components, luego de crear este nuevo directorio lo que haremos sera dirigirno hacia el archivo CMakeLists.txt ubicado en la zona global del proyecto, y alli incluiremos el nuevo directorio creado "components", escribiendo dentro del CMakeLists el siguiente comando "list(APPEND EXTRA_COMPONENT_DIRS components)" y asi guardamos el cambio mediante el comando CTRL+S, luego procedemos a la creacion de cada uno de los componentes necesarios para crear un cliente mqtt en el esp32 que permita habilitar una conexion a Azure IoT Central en una aplicacion IoT.

# CREACIOON DE COMPONENTES DENTRO DE LA CARPETA "COMPONENTS"

Dirigirse al terminal de esp-idf y escribir el siguiente comando en el terminal "cd components" para ubicarse dentro del directorio incluido en el CMakelists.txt global, luego de ello en el mismo terminal escriba el siguiente comando "idf.py create-component nombre_del_componente", posterior a esta accion se creara una carpeta con el nombre del componente dentro del directorio "components", la carpeta creada recientemente con el nombre del componente contiene una subcarpeta denominada "include" la cual contiene el header de nuestro componente o en este caso de ejemplo nombre_del_componente.h, luego encontramos un archivo denominado nombre_del_componente.c.

# ERROR A LA HORA DE INCLUIR COMPONENTES EXTERNOS O RECIEN CREADOS EN EL ARCHHIVO PRINCIPAL.

Luego de haber creado nuestro primer componente lo natural sera tratar de incluir como una libreria desde el archivo "app_main.c" el componente que creamos de la siguiente forma "#include nombre_del_componente.h", lo cual cuando construimos el proyecto nos genera un error; si revisamos detenidamente nuestro componente puede estar solicitando librerias internas de esp-idf u solicitando otro componente creado en la libreria del componente que deseamos usar en el main, para solucionar este error debemos establecer el requerimento dentro del archivo CMakelists.txt del propio componente escriibiendo el siguiente comando <-REQUIRES "az_result"-> (copiar sin los guiones y signos de mayor-menor)
luego de esto se guarda el cambio y se debe dar clic a la opcion ESP-IDF FULL CLEAN cada vez que actualizamos el CMakelists.txt en cada componente y se contruye nuevamente lo que permite al ninja.build contruir sin conflictos el proyecto.

# LIBRERIAS DE AZURE_IOT_CENTRAL.

En este codigo se contruyen los componentes necesarios para desarrollar un cliente mqtt que se aprovisiona a Azure IoT central es importante resaltar que cada una de las librerias son escenciales para la creacion de una plantilla o "Azure_PnP_Template" el cual es un modelo que permite desarrollar aplicaciones IoT de forma rapida y sencilla en el lenguaje de C.

# DESCRIPCION DE CODIGO DE APROVISIONAMIENTO INTEGRANDO MQTT + AZURE + ESP_NOW
Inicialmente se incluyen todas la librerias necesarias para usar los recursos de azure como componentes esp-idf

#include "stdio.h"
#include "stdint.h"
#include "stddef.h"
#include "string.h"
#include "esp_wifi.h"
#include "esp_now.h" ---> Para recibir informacion de un nodo ESP_NOW
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "time.h"

#include "az_core.h"
#include "az_iot.h"
#include "azure_ca.h"

#include "mbedtls/base64.h" -----> Librerias para gestionar la seguridad de conexion TLS base64 
#include "mbedtls/md.h"
#include "mbedtls/sha256.h"

#include "Azure_IoT.h" -----------> Archivos para la implementacion de plantillas para una aplicacion IoT Central P&P.
#include "Azure_IoT_PnP_Template.h"
#include "iot_configs.h"

>>Este es el manejador de eventos mqtt que usaremos del framework para el envio de datos a Azure IoT central.
<static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)>


