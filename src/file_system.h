#ifndef fileSystem_h
#define fileSystem_h

#include <strings.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

/* Function Prototypes */
bool loadDeviceSettings();
void saveWiFiConfig(String ssid, String psw, String apSSID, String apPSW);
bool loadWiFiConfig();
void saveMQTTConfig(String mqttServer, uint16_t mqttPort, String mqttUser, String mqttPsw);
bool loadMQTTConfig();
void saveAPIConfig(String apikey, String phoneNumber);
bool loadAPIConfig();

bool beginFileSystem();
String fileRead(String file);
bool fileSave(String file, String data);
bool dataRead(String filePath, uint16_t *array, uint16_t array_size);
bool dataSave(String filePath, uint16_t *array, uint16_t array_size);

const uint16_t FILE_SIZE = 1024;
const char *WIFI_CONFIG_PATH = "/dvcConfig/WiFiConfig.json";
const char *MQTT_CONFIG_PATH = "/dvcConfig/mqttConfig.json";
const char *API_CONFIG_PATH = "/dvcConfig/apiConfig.json";

StaticJsonDocument<FILE_SIZE> mqttConfig;
StaticJsonDocument<FILE_SIZE> wiFiConfig;
StaticJsonDocument<FILE_SIZE> apiConfig;


bool loadDeviceSettings()
{
    loadAPIConfig();
    loadMQTTConfig();
    loadWiFiConfig();
    return true;
}
/* WiFi Config */
void saveWiFiConfig(String ssid, String psw, String apSSID, String apPsw)
{
    wiFiConfig["ssid"] = ssid;
    wiFiConfig["psw"] = psw;
    wiFiConfig["apSSID"] = apSSID;
    wiFiConfig["apPassword"] = apPsw;

    char config[FILE_SIZE];
    serializeJson(wiFiConfig, config);
    fileSave(WIFI_CONFIG_PATH, config);
}

bool loadWiFiConfig()
{
    File file = SPIFFS.open(WIFI_CONFIG_PATH, "r");
    String fileContent = fileRead(WIFI_CONFIG_PATH);
    if (deserializeJson(wiFiConfig, fileContent))
    {
        ESP_LOGE("SPIFFS", "Failed to read Wi-Fi settings file, using default configuration");
            wiFiConfig["ssid"] = "";
            wiFiConfig["psw"] = "";
            wiFiConfig["apSSID"] = "MonBot";
            wiFiConfig["apPassword"] = "12345678";
            return false;
    }
    return true;
}

/* MQTT Config */
void saveMQTTConfig(String server, uint16_t port, String user, String psw)
{
    mqttConfig["mqttServer"] = server;
    mqttConfig["mqttPort"] = port;
    mqttConfig["mqttUser"] = user;
    mqttConfig["mqttPsw"] = psw;
    char config[FILE_SIZE];
    serializeJson(mqttConfig, config);
    fileSave(MQTT_CONFIG_PATH, config);
}

bool loadMQTTConfig()
{
    File file = SPIFFS.open(MQTT_CONFIG_PATH, "r");
    String fileContent = fileRead(MQTT_CONFIG_PATH);
    if (deserializeJson(mqttConfig, fileContent))
    {
        ESP_LOGE("SPIFFS", "Failed to read MQTT settings file, using default configuration");
        mqttConfig["mqttServer"] = "";
        mqttConfig["mqttPort"] = 1883;
        mqttConfig["mqttUser"] = "";
        mqttConfig["mqttPsw"] = "";
        return false;
    }
    return true;
}

/* API Config*/
void saveAPIConfig(String apikey, String phoneNumber)
{
    apiConfig["apiKey"] = apikey;
    apiConfig["phoneNumber"] = phoneNumber;
    char config[FILE_SIZE];
    serializeJson(apiConfig, config);
    fileSave(API_CONFIG_PATH, config);

}

bool loadAPIConfig()
{
    File file = SPIFFS.open(API_CONFIG_PATH, "r");
    String fileContent = fileRead(API_CONFIG_PATH);
    if (deserializeJson(apiConfig, fileContent))
    {
        ESP_LOGE("SPIFFS", "Failed to read API settings file, using default configuration");
        apiConfig["apiKey"] = "";
        apiConfig["phoneNumber"] = "";
        return false;
    }
    return true;
}

bool beginFileSystem()
{
    return SPIFFS.begin(true);
}

String fileRead(String file)
{
    File fileObj = SPIFFS.open(file, "r");
    return fileObj.readString();
}

bool fileSave(String file, String data)
{
    File fileObj = SPIFFS.open(file, "w+");
    return (fileObj.print(data));
}

bool dataRead(String filePath, uint16_t *array, uint16_t array_size)
{
    File file = SPIFFS.open(filePath, "r");
    uint16_t bytesLidos = file.read((uint8_t *)array, array_size * 2);
    return (bytesLidos == (2 * array_size));
}

bool dataSave(String filePath, uint16_t *array, uint16_t array_size)
{
    File file = SPIFFS.open(filePath, "w+");
    return (file.write((uint8_t *)array, (array_size * 2)));
}

#endif