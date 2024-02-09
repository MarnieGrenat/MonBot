#ifndef fileSystem_h
#define fileSystem_h

#include <strings.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

/* Function Prototypes */
void saveWiFiConfig(String ssid, String psw, String apSSID, String apPSW);
void loadWiFiConfig();
void saveMQTTConfig(String mqttServer, uint16_t mqttPort, String mqttUser, String mqttPsw);
void loadMQTTConfig();
bool beginFileSystem();
String fileRead(String file);
bool fileSave(String file, String data);
bool dataRead(String filePath, uint16_t *array, uint16_t array_size);
bool dataSave(String filePath, uint16_t *array, uint16_t array_size);


const uint16_t FILE_SIZE = 2 * 1024;
const char *WIFI_CONFIG_PATH = "/dvcConfig/WiFiConfig.json";
const char *MQTT_CONFIG_PATH = "/dvcConfig/mqttConfig.json";

StaticJsonDocument<FILE_SIZE> mqttConfig;
StaticJsonDocument<FILE_SIZE> wiFiConfig;

/* WiFi Config */

void saveWiFiConfig(String ssid, String psw, String apSSID, String apPsw)
{
    wiFiConfig["ssid"] = ssid;
    wiFiConfig["psw"] = psw;
    wiFiConfig["apSSID"] = apSSID;
    wiFiConfig["apPassword"] = apPsw;

    char config[FILE_SIZE];
    serializeJson(wiFiConfig, config);
    fileSave(MQTT_CONFIG_PATH, config);
}

void loadWiFiConfig()
{
    File file = SPIFFS.open(WIFI_CONFIG_PATH, "r");
    String fileContent = fileRead(WIFI_CONFIG_PATH);
    if (deserializeJson(wiFiConfig, fileContent))
    {
        Serial.println("Failed to read file, using default configuration");
        wiFiConfig["ssid"] = "";
        wiFiConfig["password"] = "";
        wiFiConfig["apSSID"] = "MonBot_AP";
        wiFiConfig["apPassword"] = "123456789";
    }
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
void loadMQTTConfig()
{
    File file = SPIFFS.open(MQTT_CONFIG_PATH, "r");
    String fileContent = fileRead(MQTT_CONFIG_PATH);
    if (deserializeJson(mqttConfig, fileContent))
    {
        Serial.println("Failed to read file, using default configuration");
        mqttConfig["mqttServer"] = "";
        mqttConfig["mqttPort"] = 1883;
        mqttConfig["mqttUser"] = "";
        mqttConfig["mqttPsw"] = "";
    }
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