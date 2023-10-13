| Supported Targets | ESP32-WROVER E | ESP32-S3 |
| ----------------- | ----- | -------- | 

### Descripción del Proyecto.
Este es un proyecto IoT que tiene como objetivo el desarrollo de comunicacion IoT efectiva entre un nodo de replica y un dispositivo gateway; este desarrollo tiene un enfoque particular de aplicacion;la arquitectura IoT tiene como promesa de valor la adquisicion de eventos relacionados con la actividad animal en estructuras industriales para disminuir riesgos de falla.

La arquitectura IoT propuesta establece el aprovisionamiento de datos de forma segura mediante protocolos de comunicacion TLS en recursos de Azure Microsoft, especificamente el recurso de Azure IoT Central como medio de integracion para el dispositivo fisico Esp32 Wrover-E.

### Requisitos Previos.

Para el desarrollo de este proyecto se requiere de forma previa la disposicion de componentes fisicos especificamente el dispositivo LilyGO T-Internet-COM con un microcontrolador de la serie ESP32 Wrover-E.


![](Gateway.jpg)

Se recomienda obtener un cable de conexion Ethernet configurado en Ethernet direct y que ademas cumpla con una especificacion CAT 7 la cual permite la conexion de dispositivos de alta velocidad con dispositivos de baja velocidad.

![](Ethernet_CAT.jpg)

Ademas es importante tener instalado de forma previa el entorno de desarrollo de software Visual Studio Code y posteriormente instalar el framework ESP-IDF como una extension de Visual Studio Code.

## RECOMENDACION: Permisos de conectividad y acceso a redes LAN.

Es importante aclarar que la gestion de credenciales en redes privadas optimiza la seguridad de conectividad para la solucion IoT, esto supone agregar ajustes a nuestro codigo de aprovisionamiento como establecer la IP del dispositivo en la red, capturandola de forma inicial mediante una conexion DHCP, gestionar la IP mediante mesas de servicio es escencial para obtener una conexion ethernet exitosa con el T-Internet COM, ademas es importante establecer que los puertos requeridos para esta aplicacion estan descritos de la siguiente manera:

#### MQTT_CLIENT_:8884
#### HTTP_CLIENT: 8080
#### IP: Obtenida del dispositivo.
#### MAC_Address: Obtenida del dispositivo.

## Montaje de conexion Ethernet.

Para el desarrollo de esta actividad fue necesario consultar diferentes fuentes de referencia con el fin de obtener información técnica sobre los modos de operación de la placa Espressif, se consultaron específicamente los modelos de conexión interna y SPI para conocer la interacción entre el controlador ESP y el módulo PHY, luego se procede a verificar la funcionalidad de conexión mediante códigos de ejemplo proporcionados por el fabricante de la placa, después se determinó a partir del pinout del controlador la disponibilidad de pines para el funcionamiento correcto y estable del módulo y su estado de conexión exitoso inicializando así el módulo PHY, finalmente mediante integración de información tanto desde el fabricante como plantillas predefinidas de prueba se genera un código individual para la placa adquirida, como último paso se flashea el dispositivo y  obtenemos parámetros mediante puerto serial como la dirección IP, los puertos requeridos para la transacción de información en función del protocolo mqtt, y la dirección MAC STA ETH del dispositivo como se indico en el punto anterior.

![](con1_eth.jpg)

El estado de blink en el led de color verde indica estado de conexion exitoso.


![](Gateway_photo.jpg)

El dispositivo Espressif ESP32 T-Internet COM admite una alimentacion desde 5V hasta 12 V con una corriente minima de 2 A, es recomendable alimentar el modulo con una fuente regulada a una tension directa de 12V debido a que los protocolo de transmision y recepcion de datos asociado a la conexion ethernet o transmision LTE 4G demanda picos de tension de hasta 12V con corrientes  asociadas a un rango de hasta 2A.


<h1 align = "center">LilyGo T-Internet-COM</h1>

En este proyecto se usa un dispositivo Espressif con referencia de microcontrolador ESP_WROVER E fabricado por LilyGo, el cual posee capacidades para el procesamiento de conexiones Ethernet mediante la habilitacion interna de un PHY LAN8720 definiendo los pines descritos en la siguiente imagen.

#### Diagrama de dispositivo LilyGO T-Internet

![](T-Internet-COM.jpg)

# Caracteristicas tecnicas.


## Conectividad: 

El dispositivo suministra información a la nube mediante una red LAN a través de un puerto de conexión Ethernet

-------------------------------------------------------------------------------

## Actualización del Firmware: 

UART descarga/OTA (a través de la red/host para descargar y escribir firmware)

-------------------------------------------------------------------------------

## Tipo de cifrado: 

AES/RSA/ECC/SHA

-------------------------------------------------------------------------------

## Mecanismo de seguridad:

WPA/WPA2/WPA2-Enterprise/WPS

-------------------------------------------------------------------------------
## Protocolo de red: 
IPv4, IPv6, SSL, TCP/UDP/HTTP/FTP/MQTT

-------------------------------------------------------------------------------
## Desarrollo de Software: 
Admite desarrollo de servidores en la nube/SDK para el desarrollo de firmware de usuario

-------------------------------------------------------------------------------
## Configuración de usuario: 
Conjunto de instrucciones AT +, servidor en la nube, android/iOS app

-------------------------------------------------------------------------------
## Estándar: 
FCC/CE-RED/IC/TELEC/KCC/SRRC/NCC (esp32 chip)

-------------------------------------------------------------------------------
## Velocidad: 
802.11 b/g/n (802.11n, velocidad de hasta 150mbps) A-MPDU y polimerización de A-MSDU, soporte

-------------------------------------------------------------------------------
## Intervalo de proteccion: 
0,4μs

-------------------------------------------------------------------------------
## Rango de frecuencia: 
2.4 GHz ~ 2.5GHz (2400 M ~ 2483,5M

-------------------------------------------------------------------------------
## Potencia de transmisión: 
22dBm

-------------------------------------------------------------------------------
## Distancia de comunicación: 
300m

-------------------------------------------------------------------------------

# Especificaciones tecnicas para los modos de conexion en protocolos de comunicacion IoT.

#### HALF_DUPLEX - FULL_DUPELX.


<h1 align = "center">Conexiones Full Duplex</h1>

#### Autenticación Fuerte:
Dado que ambas direcciones pueden transmitir simultáneamente, la autenticación sólida es fundamental para asegurarse de que ambos extremos de la comunicación sean legítimos.
#### Cifrado Continuo:
Para garantizar la confidencialidad, se debe utilizar cifrado continuo, como TLS/SSL, que cifra los datos en ambas direcciones de la comunicación.
#### Gestión de Ancho de Banda:
Implementar políticas de control de ancho de banda para evitar abusos y garantizar un rendimiento justo en la red.
#### Detección de Ataques:
Utilizar herramientas de detección de intrusos y sistemas de prevención de intrusiones para detectar y mitigar ataques en tiempo real. 
                                          
### Configuracion de disposicion Ethernet.

Se utiliza un cable ethernet con jacks macho macho de referencia RJ45, con la siguiente configuración CAT7 disponible para velocidades de transmisión de hasta 10 GHZ, compatible con dispositivos rápidos y lentos, la configuración del cable ethernet es estándar y se desarrolla según la siguiente imagen 2.

![](ethernet.jpg)


## Configuración del Entorno de Desarrollo.

Instrucciones para la configuracion del entorno en un framework ESP-IDF para la integracion compatibilizada de funciones para microcontroladores de la serie Wrover-E adecuadndo la recepcion de telemetria mediante protocolos IoT y el aprovsionamiento de data mediante un cliente mqtt con transporte TLS mediante el puerto 8884, en Azure IoT Central.

## Disposisioion de cabecera para la activacion de comunicacion Ethernet:

### PASO_1 Obtención de una plantilla personalizada.
Se estableció una plantilla personalizada en base a la configuración de la PHY ADDR adecuada para la capa física LAN8720, esto se genera en base al pinout definido por el fabricante, la plantilla es única para cada dispositivo y se debe verificar que sea activada.

Mediante el framework ESP-IDF se genera la activación de plantilla a través del comando ```idf.py menuconfig```

![](MENUCONFIG.jpg)

### PASO_2 Configuración.
Luego se selecciona la opción Example Configuration y se asignan los pines correctos y el modo de conexión.

![](paso2.jpg)

### PASO_3 Activacion de pines.

Se habilita la plantilla mediante el guardado y cargado de código, luego con la plantilla definida en el framework cargamos el código al dispositivo siempre definiendo la configuracion de conexion al PHY.

```
#define ETH_POWER_PIN 4
#define ETH_TYPE ETH_PHY_LAN8720
#define ETH_ADDR 0
#define ETH_MDC_PIN 23
#define ETH_MDIO_PIN 18
```


# Arquitectura IoT Azure

![](diagrama.jpg)

### CREAR LIBRERIAS AZURE EN ESP_IDF.

Para crear las librerias como componentes externos de azure inicialmente se dispone la creacion de un nuevo directorio o carpeta dentro del proyecto, denominada components, luego de crear este nuevo directorio lo que haremos sera dirigirno hacia el archivo CMakeLists.txt ubicado en la zona global del proyecto, y alli incluiremos el nuevo directorio creado "components", escribiendo dentro del CMakeLists el siguiente comando ```list(APPEND EXTRA_COMPONENT_DIRS components)``` y asi guardamos el cambio mediante el comando CTRL+S, luego procedemos a la creacion de cada uno de los componentes necesarios para crear un cliente mqtt en el esp32 que permita habilitar una conexion a Azure IoT Central en una aplicacion IoT.

### CREACION DE COMPONENTES DENTRO DE LA CARPETA "COMPONENTS".

Dirigirse al terminal de esp-idf y escribir el siguiente comando en el terminal "cd components" para ubicarse dentro del directorio incluido en el CMakelists.txt global, luego de ello en el mismo terminal escriba el siguiente comando "idf.py create-component nombre_del_componente", posterior a esta accion se creara una carpeta con el nombre del componente dentro del directorio "components", la carpeta creada recientemente con el nombre del componente contiene una subcarpeta denominada "include" la cual contiene el header de nuestro componente o en este caso de ejemplo nombre_del_componente.h, luego encontramos un archivo denominado nombre_del_componente.c.

![](instruccion.jpg)


### ERROR A LA HORA DE INCLUIR COMPONENTES EXTERNOS O RECIEN CREADOS EN EL ARCHIVO PRINCIPAL.

Luego de haber creado nuestro primer componente lo natural sera tratar de incluir como una libreria desde el archivo ``` app_main.c ```el componente que creamos de la siguiente forma ```#include nombre_del_componente.h```, lo cual cuando construimos el proyecto nos genera un error; si revisamos detenidamente nuestro componente puede estar solicitando librerias internas de esp-idf u solicitando otro componente creado en la libreria del componente que deseamos usar en el main, para solucionar este error debemos establecer el requerimento dentro del archivo CMakelists.txt del propio componente escriibiendo el siguiente comando ```REQUIRES "az_result"``` (copiar sin los guiones y signos de mayor-menor)
luego de esto se guarda el cambio y se debe dar clic a la opcion ESP-IDF FULL CLEAN cada vez que actualizamos el CMakelists.txt en cada componente y se contruye nuevamente lo que permite al ninja.build contruir sin conflictos el proyecto.

### LIBRERIAS DE AZURE_IOT_CENTRAL.

En este codigo se contruyen los componentes necesarios para desarrollar un cliente mqtt que se aprovisiona a Azure IoT central es importante resaltar que cada una de las librerias son escenciales para la creacion de una plantilla o "Azure_PnP_Template" el cual es un modelo que permite desarrollar aplicaciones IoT de forma rapida y sencilla en el lenguaje de C.

### DESCRIPCION DE CODIGO DE APROVISIONAMIENTO INTEGRANDO MQTT + AZURE + ESP_NOW.
Inicialmente se incluyen todas la librerias necesarias para usar los recursos de azure como componentes esp-idf
```
//...

##### #include "esp_now.h" ---> Para recibir informacion de un nodo ESP_NOW

//....

##### #include "protocol_examples_common.h"

//...

##### #include "esp_log.h"
##### #include "mqtt_client.h"
##### #include "time.h"

//....
##### #include "Azure_IoT.h" ---> Archivos para la implementacion de plantillas para una aplicacion IoT Central P&P.
##### #include "Azure_IoT_PnP_Template.h"
##### #include "iot_configs.h"
##### #include "az_core.h"
##### #include "az_iot.h"
##### #include "azure_ca.h"
//.....

##### #include "mbedtls/base64.h" -----> Librerias para gestionar la seguridad de conexion TLS base64 
```



Este es el manejador de eventos mqtt que usaremos del framework para el envio de datos a Azure IoT central.
```
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)>
```

## Creacion de un cliente MQTT que permita transportar telemetria a Azure.

Para establecer la funcion static int mqtt_client_init_function(mqtt_client_config_t *mqtt_client_config,  mqtt_client_handle_t *mqtt_client_handle) de forma funcional y llamar en cada Switch del manejador de eventos; 

#### 1) Se deben declarar como estructura las credenciales para el transporte MQTT y generar los payloads correspondientes. 

#### 2) Para ello se deben instanciar variables de forma inicial como estructuras.

#### 3) Definir como punteros AZ_SPAN (los cuales se deben redifinir #define AZ_IOT_DATA_BUFFER_SIZE), esto permite usar la posicion de memoria flotante optimizando el rendimiento de codigo. 

## ESTRUCTURAS:

>##### struct authentication_t ------> Para usar todas las credenciales de password
>##### struct session_t -------> Para usar estructuras que permiten verificar el estado de session mqtt
>##### struct network_t -------> Para verficar el estado de conexion.
>##### struct credentials_t --------> Para usar estructuras que permiten usar como un puntero AZ_SPAN como username.
>##### esp_http_client_config_t mqtt_cnfi; ------> Para usar estructuras que permiten validar certificaciones pem.
>##### struct json_n mqtt_cfi; -------> Un puntero a un contexto proporcionado por el usuario que se utiliza en conjunto con la función allocator_callback. Puede contener información adicional proporcionada por el usuario.

La estructura <az_json_writer> es parte de una biblioteca de escritura de JSON. Esta estructura se utiliza para llevar un seguimiento del estado y los detalles de la escritura de datos JSON en una secuencia de bytes o en un búfer de destino.

## Ejemplo de Salida en el monitor serial.

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


## DESARROLLO DE APROVISIONAMIENTO AZURE.
### (PRUEBA DE ESTADO DE TRANSMISIÓN JSON + ETHERNET )

![](azconn.jpg)






