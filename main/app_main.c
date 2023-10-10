/* MQTT (over TCP) Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_eth.h"
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
#include "sys/time.h"

#include "az_core.h"
#include "az_iot.h"
#include "azure_ca.h"

#include "mbedtls/base64.h"
#include "mbedtls/md.h"
#include "mbedtls/sha256.h"

#include "Azure_IoT.h"
#include "Azure_IoT_PnP_Template.h"
#include "iot_configs.h"

#define SERIAL_LOGGER_BAUD_RATE 115200
#define MQTT_DO_NOT_RETAIN_MSG 0

#define NTP_SERVERS "pool.ntp.org", "time.nist.gov"

#define PST_TIME_ZONE -8
#define PST_TIME_ZONA_DAYLIGHT_SAVINGS_DIFF 1

#define GMT_OFFSET_SECS (PST_TIME_ZONE * 3600) //////////// se define lazona horaria y se establece la unidad de segundo
#define GMT_OFFSET_SECS_DST ((PST_TIME_ZONE + PST_TIME_ZONA_DAYLIGHT_SAVINGS_DIFF))

#define UNIX_TIME_OCT_09_2023 1510592825
#define UNIX_EPOCH_START_YEAR 2000

/*FUNCIONES QUE RETORNAN VALORES*/

#define RESULT_OK 0
#define RESULT_ERROR __LINE__

static const char *TAG = "MQTT_AZURE_IOT_CENTRAL";

static const char *wifi_ssid = IOT_CONFIG_WIFI_SSID;
static const char *wifi_password = IOT_CONFIG_WIFI_PASSWORD;

/*DECLARACION DE FUNCIONES PARA EL USO DE TEMPLATES*/
// This is a logging function used by Azure IoT client.
static void logging_function(log_level_t log_level, char const *const format, ...);

/*INSTANACIAMOS VARIABLES COMO CLASES DE AZURE Y LES ASIGNAMOS UN NOMBRE CON EL CUAL INTEGRAMOS LA API REST*/
static azure_iot_config_t azure_iot_config;
static azure_iot_t azure_iot;
static esp_mqtt_client_handle_t mqtt_client;
static esp_err_t esp_mqtt_event_handler(esp_mqtt_event_handle_t event);
#define MQTT_OVER_TCP_SCHEME "mqtt"
///////////////////////////////////////////////////////////////////////
static esp_err_t esp_mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    switch (event->event_id)
    {
    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT client in ERROR state.");
        ESP_LOGE(TAG,
                 "esp_tls_stack_err=%d; "
                 "esp_tls_cert_verify_flags=%d; esp_transport_sock_errno=%d; error_type=%d; connect_return_code=%d",
                 event->error_handle->esp_tls_stack_err,
                 event->error_handle->esp_tls_cert_verify_flags,
                 event->error_handle->esp_transport_sock_errno,
                 event->error_handle->error_type,
                 event->error_handle->connect_return_code);

        switch (event->error_handle->connect_return_code)
        {
        case MQTT_CONNECTION_ACCEPTED:
            ESP_LOGE(TAG, "connect_return_code=MQTT_CONNECTION_ACCEPTED");
            break;
        case MQTT_CONNECTION_REFUSE_PROTOCOL:
            ESP_LOGE(TAG, "connect_return_code=MQTT_CONNECTION_REFUSE_PROTOCOL");
            break;
        case MQTT_CONNECTION_REFUSE_ID_REJECTED:
            ESP_LOGE(TAG, "connect_return_code=MQTT_CONNECTION_REFUSE_ID_REJECTED");
            break;
        case MQTT_CONNECTION_REFUSE_SERVER_UNAVAILABLE:
            ESP_LOGE(TAG, "connect_return_code=MQTT_CONNECTION_REFUSE_SERVER_UNAVAILABLE");
            break;
        case MQTT_CONNECTION_REFUSE_BAD_USERNAME:
            ESP_LOGE(TAG, "connect_return_code=MQTT_CONNECTION_REFUSE_BAD_USERNAME");
            break;
        case MQTT_CONNECTION_REFUSE_NOT_AUTHORIZED:
            ESP_LOGE(TAG, "connect_return_code=MQTT_CONNECTION_REFUSE_NOT_AUTHORIZED");
            break;
        default:
            ESP_LOGE(TAG, "connect_return_code=unknown (%d)", event->error_handle->connect_return_code);
            break;
        };
        break;
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT client connected (session_present=%d).", event->session_present);

        if (azure_iot_mqtt_client_connected(&azure_iot) != 0)
        {
            ESP_LOGE(TAG, "azure_iot_mqtt_client_connected failed.");
        }
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT client disconnected.");

        if (azure_iot_mqtt_client_disconnected(&azure_iot) != 0)
        {
            ESP_LOGE(TAG, "azure_iot_mqtt_client_disconnected failed.");
        }
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT topic subscribed (message id=%d).", event->msg_id);

        if (azure_iot_mqtt_client_subscribe_completed(&azure_iot, event->msg_id) != 0)
        {
            ESP_LOGE(TAG, "azure_iot_mqtt_client_subscribe_completed failed.");
        }
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT topic unsubscribed.");
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT event MQTT_EVENT_PUBLISHED");

        if (azure_iot_mqtt_client_publish_completed(&azure_iot, event->msg_id) != 0)
        {
            ESP_LOGE(TAG, "azure_iot_mqtt_client_publish_completed failed (message id=%d).", event->msg_id);
        }
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT message received.");

        mqtt_message_t mqtt_message;
        mqtt_message.topic = az_span_create((uint8_t *)event->topic, event->topic_len);
        mqtt_message.payload = az_span_create((uint8_t *)event->data, event->data_len);
        //mqtt_message.qos = MQTT_QOS_0; // Ajusta la calidad de servicio según sea necesario

        if (azure_iot_mqtt_client_message_received(&azure_iot, &mqtt_message) != 0)
        {
            ESP_LOGE(TAG, "azure_iot_mqtt_client_message_received failed (topic=%.*s).", event->topic_len, event->topic);
        }
        break;
    case MQTT_EVENT_BEFORE_CONNECT:
        ESP_LOGI(TAG, "MQTT client connecting.");
        break;
    default:
        ESP_LOGE(TAG, "MQTT event UNKNOWN.");
        break;
    }

    return ESP_OK;
}

////////////////////////////////////////////////////////////////////////7

static char mqtt_broker_uri[128];
#define AZ_IOT_DATA_BUFFER_SIZE 1500
static uint8_t az_iot_data_buffer[AZ_IOT_DATA_BUFFER_SIZE];

#define MQTT_PROTOCOL_PREFIX "mqtts://"
static uint32_t properties_request_id = 0;
static bool send_device_info = true;
static bool azure_initial_connect = false; ///// MEDIANTE ESTE PARAMETRO GESTIONAMOS LA CONEXION DE AZURE Se vuelve verdadero cuando el ESP32 se conecta correctamente a Azure IoT Central por primera vez
////////////////////////// FUNCIONES DECLARADAS /////////////////////////////

static void logging_function(log_level_t log_level, char const *const format, ...) //////// funcion que gestsiona el tiempo de conexion del dispositivo a Azure
{
    struct tm *ptm;
    time_t now = time(NULL);
    ptm = gmtime(&now);

    if (ptm->tm_hour < 10)
    {
        LogInfo(0);
        /* code */
    }

    if (ptm->tm_min < 10)
    {
        LogInfo(0);
        /* code */
    }
    LogInfo(ptm->tm_hour);
    LogInfo(":");

    if (ptm->tm_sec < 10)
    {
        LogInfo(0);
    }

    char message[256];
    va_list ap;
    va_start(ap, format);
    int message_length = vsnprintf(message, 256, format, ap);
    va_end(ap);
}

static void sync_device_clock_with_ntp_server();

static void sync_device_clock_with_ntp_server()
{
    LogInfo("Setting time using SNTP");

    // configTime(GMT_OFFSET_SECS, GMT_OFFSET_SECS_DST, NTP_SERVERS);
    time_t now = time(NULL);
    while (now < UNIX_TIME_OCT_09_2023)
    {

        now = time(NULL);
    }

    LogInfo("Time initialized!");
}

// static void connect_to_wifi();
// static esp_err_t esp_mqtt_event_handler (esp_mqtt_event_handle_t event)

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

int esp_mqtt_client_publish(esp_mqtt_client_handle_t client, const char *topic,
                            const char *data, int len, int qos, int retain);

struct authentic_t
{

    const char *password;
};

static int mqtt_client_init_function(
    mqtt_client_config_t *mqtt_client_config,
    mqtt_client_handle_t *mqtt_client_handle)
{
    int result;
    esp_mqtt_client_config_t mqtt_config;
    memset(&mqtt_config, 0, sizeof(mqtt_config));

    az_span mqtt_broker_uri_span = AZ_SPAN_FROM_BUFFER(mqtt_broker_uri);
    mqtt_broker_uri_span = az_span_copy(mqtt_broker_uri_span, AZ_SPAN_FROM_STR("https://natio.azureiotcentral.com"));
    mqtt_broker_uri_span = az_span_copy(mqtt_broker_uri_span, mqtt_client_config->address);
    az_span_copy_u8(mqtt_broker_uri_span, null_terminator);

    //mqtt_config.uri = mqtt_broker_uri;
    //mqtt_config.port = mqtt_client_config->port;
    //mqtt_config.client_id = (const char *)az_span_ptr(mqtt_client_config->client_id);
    //mqtt_config.username = (const char *)az_span_ptr(mqtt_client_config->username);

#ifdef IOT_CONFIG_USE_X509_CERT
    LogInfo("MQTT client using X509 Certificate authentication");
    mqtt_config.client_cert_pem = IOT_CONFIG_DEVICE_CERT;
    mqtt_config.client_key_pem = IOT_CONFIG_DEVICE_CERT_PRIVATE_KEY;
#else // Using SAS key

    struct authentication_t auth;
    auth.password = (const char *)az_span_ptr(mqtt_client_config->password);
    // mqtt_config.password = (const char *)az_span_ptr(mqtt_client_config->password);

#endif

    // auth.password=(const char *)az_span_ptr(mqtt_client_config->password);
    // session_t.mqtt_config.keepalive=30;
    // keepalive = 30;

    // int esp_mqtt_client_config_t -> session_t->
    // mqtt_config.disable_clean_session = 0;
    // mqtt_config.disable_auto_reconnect = false;
    // esp_mqtt_event_handle_t esp_mqtt_client_init(&mqtt_config);
    // mqtt_config.user_context = NULL;
    // mqtt_config.cert_pem = (const char *)ca_pem;

    LogInfo("MQTT client target uri set to '%s'", mqtt_broker_uri);

    mqtt_client = esp_mqtt_client_init(&mqtt_config);

    if (mqtt_client == NULL)
    {
        LogError("esp_mqtt_client_init failed.");
        result = 1;
    }
    else
    {
        esp_err_t start_result = esp_mqtt_client_start(mqtt_client);

        if (start_result != ESP_OK)
        {
            LogError("esp_mqtt_client_start failed (error code: 0x%08x).", start_result);
            result = 1;
        }
        else
        {
            *mqtt_client_handle = mqtt_client;
            result = 0;
        }
    }

    return result;
}

/////////////////////// Publicador de eventos mqtt ////////////////////////////////////

static int mqtt_client_publish_function(
    mqtt_client_handle_t mqtt_client_handle,
    mqtt_message_t *mqtt_message)
{
    LogInfo("MQTT client publishing to '%s'", az_span_ptr(mqtt_message->topic));

    int mqtt_result = esp_mqtt_client_publish(
        (esp_mqtt_client_handle_t)mqtt_client_handle,
        (const char *)az_span_ptr(mqtt_message->topic), // topic is always null-terminated.
        (const char *)az_span_ptr(mqtt_message->payload),
        az_span_size(mqtt_message->payload),
        (int)mqtt_message->qos,
        MQTT_DO_NOT_RETAIN_MSG);

    if (mqtt_result == -1)
    {
        return RESULT_ERROR;
    }
    else
    {
        return RESULT_OK;
    }
}

///////////////////////  SEGURIDAD CON GENERACION DE CERTIFICADOS SHA256  //////////////////////////////////////
uint8_t byte;
static int mbedtls_hmac_sha256(
    const uint8_t *key,
    size_t key_length,
    const uint8_t *payload,
    size_t payload_length,
    uint8_t *signed_payload,
    size_t signed_payload_size)
{
    (void)signed_payload_size;
    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
    mbedtls_md_hmac_starts(&ctx, (const unsigned char *)key, key_length);
    mbedtls_md_hmac_update(&ctx, (const unsigned char *)payload, payload_length);
    mbedtls_md_hmac_finish(&ctx, signed_payload);
    mbedtls_md_free(&ctx);

    return 0;
}

static int base64_decode(
    uint8_t *data,
    size_t data_length,
    uint8_t *decoded,
    size_t decoded_size,
    size_t *decoded_length)
{
    return mbedtls_base64_decode(decoded, decoded_size, decoded_length, data, data_length);
}

// Ver la documentacion de `base64_encode_function_t` en AzureIoT.h para mas detalles.

static int base64_encode(
    uint8_t *data,
    size_t data_length,
    uint8_t *encoded,
    size_t encoded_size,
    size_t *encoded_length)
{
    return mbedtls_base64_encode(encoded, encoded_size, encoded_length, data, data_length);
}

static void on_properties_update_completed(uint32_t request_id, az_iot_status status_code)
{
    LogInfo("Properties update request completed (id=%d, status=%d)", request_id, status_code);
}

void on_properties_received(az_span properties)
{
    LogInfo("Properties update received: %.*s", az_span_size(properties), az_span_ptr(properties));

    // It is recommended not to perform work within callbacks.
    // The properties are being handled here to simplify the sample.
    if (azure_pnp_handle_properties_update(&azure_iot, properties, properties_request_id++) != 0)
    {
        LogError("Failed handling properties update.");
    }
}

static void on_command_request_received(command_request_t command)
{
    az_span component_name = az_span_size(command.component_name) == 0 ? AZ_SPAN_FROM_STR("") : command.component_name;

    LogInfo(
        "Command request received (id=%.*s, component=%.*s, name=%.*s)",
        az_span_size(command.request_id),
        az_span_ptr(command.request_id),
        az_span_size(component_name),
        az_span_ptr(component_name),
        az_span_size(command.command_name),
        az_span_ptr(command.command_name));

    // Here the request is being processed within the callback that delivers the command request.
    // However, for production application the recommendation is to save `command` and process it
    // outside this callback, usually inside the main thread/task/loop.
    (void)azure_pnp_handle_command_request(&azure_iot, command);
}

static int mqtt_client_subscribe_function(
    mqtt_client_handle_t mqtt_client_handle,
    az_span topic,
    mqtt_qos_t qos)
{
    LogInfo("MQTT client subscribing to '%.*s'", az_span_size(topic), az_span_ptr(topic));

    // As per documentation, `topic` always ends with a null-terminator.
    // esp_mqtt_client_subscribe returns the packet id or negative on error already, so no conversion
    // is needed.
    int packet_id = esp_mqtt_client_subscribe(
        (esp_mqtt_client_handle_t)mqtt_client_handle, (const char *)az_span_ptr(topic), (int)qos);

    return packet_id;
}

static int mqtt_client_deinit_function(mqtt_client_handle_t mqtt_client_handle)
{
    int result = 0;
    esp_mqtt_client_handle_t esp_mqtt_client_handle = (esp_mqtt_client_handle_t)mqtt_client_handle;

    LogInfo("MQTT client being disconnected.");

    if (esp_mqtt_client_stop(esp_mqtt_client_handle) != ESP_OK)
    {
        LogError("Failed stopping MQTT client.");
    }

    if (esp_mqtt_client_destroy(esp_mqtt_client_handle) != ESP_OK)
    {
        LogError("Failed destroying MQTT client.");
    }

    if (azure_iot_mqtt_client_disconnected(&azure_iot) != 0)
    {
        LogError("Failed updating azure iot client of MQTT disconnection.");
    }

    return 0;
}
static void configure_azure_iot();
static void configure_azure_iot()
{
    /*
     * The configuration structure used by Azure IoT must remain unchanged (including data buffer)
     * throughout the lifetime of the sample. This variable must also not lose context so other
     * components do not overwrite any information within this structure.
     */
    azure_iot_config.user_agent = AZ_SPAN_FROM_STR(AZURE_SDK_CLIENT_USER_AGENT);
    azure_iot_config.model_id = azure_pnp_get_model_id();
    azure_iot_config.use_device_provisioning = true; // Required for Azure IoT Central.
    azure_iot_config.iot_hub_fqdn = AZ_SPAN_EMPTY;
    azure_iot_config.device_id = AZ_SPAN_EMPTY;

#ifdef IOT_CONFIG_USE_X509_CERT
    azure_iot_config.device_certificate = AZ_SPAN_FROM_STR(IOT_CONFIG_DEVICE_CERT);
    azure_iot_config.device_certificate_private_key = AZ_SPAN_FROM_STR(IOT_CONFIG_DEVICE_CERT_PRIVATE_KEY);
    azure_iot_config.device_key = AZ_SPAN_EMPTY;
#else
    azure_iot_config.device_certificate = AZ_SPAN_EMPTY;
    azure_iot_config.device_certificate_private_key = AZ_SPAN_EMPTY;
    azure_iot_config.device_key = AZ_SPAN_FROM_STR(IOT_CONFIG_DEVICE_KEY);
#endif // IOT_CONFIG_USE_X509_CERT

    azure_iot_config.dps_id_scope = AZ_SPAN_FROM_STR(DPS_ID_SCOPE);
    azure_iot_config.dps_registration_id = AZ_SPAN_FROM_STR(IOT_CONFIG_DEVICE_ID); // Use Device ID for Azure IoT Central.
    azure_iot_config.data_buffer = AZ_SPAN_FROM_BUFFER(az_iot_data_buffer);
    azure_iot_config.sas_token_lifetime_in_minutes = MQTT_PASSWORD_LIFETIME_IN_MINUTES;
    azure_iot_config.mqtt_client_interface.mqtt_client_init = mqtt_client_init_function;
    azure_iot_config.mqtt_client_interface.mqtt_client_deinit = mqtt_client_deinit_function;
    azure_iot_config.mqtt_client_interface.mqtt_client_subscribe = mqtt_client_subscribe_function;
    azure_iot_config.mqtt_client_interface.mqtt_client_publish = mqtt_client_publish_function;
    azure_iot_config.data_manipulation_functions.hmac_sha256_encrypt = mbedtls_hmac_sha256;
    azure_iot_config.data_manipulation_functions.base64_decode = base64_decode;
    azure_iot_config.data_manipulation_functions.base64_encode = base64_encode;
    azure_iot_config.on_properties_update_completed = on_properties_update_completed;
    azure_iot_config.on_properties_received = on_properties_received;
    azure_iot_config.on_command_request_received = on_command_request_received;

    azure_iot_init(&azure_iot, &azure_iot_config);
}

void setup()

{

    azure_pnp_init();
    configure_azure_iot();
    azure_iot_start(&azure_iot);
    LogInfo("Azure IoT client initialized (state=%d)", azure_iot.state);
}

///////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) ////////////// este es el manejador de eventos mqtt que usaremos del framework para el envio de datos a Azure IoT central
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_publish(client, "/my_topic/qos1", "data_3", 0, 1, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/my_topic/qos0", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/my_topic/qos1", 1);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_unsubscribe(client, "/my_topic/qos1");
        ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(client, "/my_topic/qos0", "data", 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static void mqtt_app_start(void)
{
  int result;
  esp_mqtt_client_config_t mqtt_config;
  memset(&mqtt_config, 0, sizeof(mqtt_config));

    esp_mqtt_client_config_t mqtt_client = {
        .broker.address.uri = mqtt_broker_uri,

    };
#if CONFIG_BROKER_URL_FROM_STDIN
    char line[128];

    if (strcmp(mqtt_client.broker.address.uri, "FROM_STDIN") == 0)
    {
        int count = 0;
        printf("https://natio.azureiotcentral.com\n");
        while (count < 128)
        {
            int c = fgetc(stdin);
            if (c == '\n')
            {
                line[count] = '\0';
                break;
            }
            else if (c > 0 && c < 127)
            {
                line[count] = c;
                ++count;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        mqtt_client.broker.address.uri = line;
        printf("Broker url: %s\n", line);
    }
    else
    {
        ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
        abort();
    }
#endif /* CONFIG_BROKER_URL_FROM_STDIN */

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_client);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler*/ 
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, esp_mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());
    // azure_iot_stop(&azure_iot);              /////////////////////////////////////
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("outbox", ESP_LOG_VERBOSE);
    // ESP_ERROR_CHECK(configure_azure_iot()); /////////////////////////////
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());
    // ESP_ERROR_CHECK(configure_azure_iot());
    mqtt_app_start();
    if (esp_wifi_connect() != ESP_OK)
    {
        if (!azure_initial_connect)
        {

            configure_azure_iot();
        }

        azure_iot_start(&azure_iot);
        // La conexión Wi-Fi no está establecida o ha fallado.
        // Realiza acciones para conectarte a una red Wi-Fi.
    }
    else

    {
        switch (azure_iot_get_status(&azure_iot)) ///////////// switch para los diferentes estados de conexion de aprovisionamiento a Azure.
        {
        case azure_iot_connected:
            azure_initial_connect = true;
            if (send_device_info)
            {
                (void)azure_pnp_send_device_info(&azure_iot, properties_request_id++);
                send_device_info = false; // solo es necesario una vez
            }
            else if (azure_pnp_send_telemetry(&azure_iot) != 0)
            {
                ESP_LOGE(TAG, "Fallo en el envio de telemtria");
            }

            // Código para el estado 'azure_iot_connected'
            break;

        case azure_iot_error:
            ESP_LOGE(TAG, "Error en la conexion a Azure");
            azure_iot_stop(&azure_iot);
            // Código para el estado 'azure_iot_error'
            break;

        case azure_iot_disconnected:
            esp_wifi_stop();

            // Código para el estado 'azure_iot_disconnected'
            // Puedes agregar aquí acciones específicas cuando se desconecta
            break;

        default:
            break;
        }

        azure_iot_do_work(&azure_iot);

        // configure_azure_iot();
        // setup(); ///////// funcion que llama a los metodos de Azure IoT central
    }
}
