#include "globals.h"
#include <WebServer.h>
#include <HTTPClient.h>
#include <UrlEncode.h>
#include "webpage.h"

/* Function Prototypes */
void initWebserver();
void configPages();
void handleRoot();
void handleNotFound();
void handleGetData();
void handleGetMqttConfig();
void handleSetMqttConfig();
void handleGetWifiConfig();
void handleSetWifiConfig();
void handleGetAPIConfig();
void handleSetAPIConfig();
bool isAPIKeyValid(String key, String phoneNumber);

void sendAlert(String message);
void configDeviceTime();
String printLocalTime(bool echo);

const char* ntpServer = "pool.ntp.org";

String apiKey;
String phoneNumber;

HTTPClient http;
WebServer webserver(80);


/* WebServer */
void initWebserver()
{
    configPages();
    webserver.begin(80);
}

void configPages()
{
    webserver.on("/", handleRoot);
    webserver.on("/getData", handleGetData);

    webserver.on("/getMqttConfig", handleGetMqttConfig);
    webserver.on("/setMqttConfig", handleSetMqttConfig);

    webserver.on("/getWiFiConfig", handleGetWifiConfig);
    webserver.on("/setWiFiConfig", handleSetWifiConfig);

    webserver.on("/getAPIConfig", handleGetAPIConfig);
    webserver.on("/setAPIConfig", handleSetAPIConfig);

    webserver.onNotFound(handleNotFound);
}

void handleRoot()
{
    const uint16_t section_length = 2 * 1024;
    uint32_t page_length = strlen(webpage);
    webserver.sendContent("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + String(page_length) + "\r\nConnection: close\r\n");
    char pt1[section_length] = "";
    uint32_t index = 0;
    while (page_length > section_length)
    {
        memcpy(pt1, webpage + index, section_length);
        index += section_length;
        webserver.sendContent(pt1, section_length);
        page_length -= section_length;
    }
    memset(pt1, 0, section_length);
    memcpy(pt1, webpage + index, page_length);
    webserver.sendContent(pt1, page_length + 1); // Send web page
    return;
}

void handleGetData()
{
    String response = "{\"localTime\":\"" + printLocalTime(0) + "\"}";
    webserver.send(200, "application/json", response);
}
void handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += webserver.uri();
    message += "\nMethod: ";
    message += (webserver.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += webserver.args();
    message += "\n";

    for (uint8_t i = 0; i < webserver.args(); i++)
    {
        message += " " + webserver.argName(i) + ": " + webserver.arg(i) + "\n";
    }
    webserver.send(404, "text/plain", message);
    return;
}

void handleGetMqttConfig()
{
    loadMQTTConfig();
    String mqttServer = mqttConfig["mqttServer"].as<String>();
    uint16_t mqttPort = mqttConfig["mqttPort"].as<uint16_t>();
    String mqttUsername = mqttConfig["mqttUsername"].as<String>();
    String mqttPassword = mqttConfig["mqttPassword"].as<String>();
    String response = "{\"mqttServer\":\"" + mqttServer + "\",\"mqttPort\":" + String(mqttPort) + ",\"mqttUsername\":\"" + mqttUsername + "\",\"mqttPassword\":\"" + mqttPassword + "\"}";
    webserver.send(200, "application/json", response);

}

void handleSetMqttConfig()
{
    String mqttSettings = webserver.arg("plain");
    if (mqttSettings == "")
    {
        webserver.send(400, "text/plain", "Formatação dos parâmetros Wi-Fi inválida.");
        return;
    }
    String mqttServer = mqttConfig["mqttServer"];
    uint16_t mqttPort = mqttConfig["mqttPort"].as<uint16_t>();
    String mqttUsername = mqttConfig["mqttUsername"];
    String mqttPassword = mqttConfig["mqttPassword"];
    ESP_LOGI("SETTING MQTT", "MQTT SERVER: %s, MQTT PORT: %u", mqttServer, mqttPort);
    ESP_LOGI("SETTING MQTT", "MQTT USERNAME: %s, MQTT PASSWORD: %u", mqttUsername, mqttPassword);

    if (mqttServer.isEmpty() || mqttUsername.isEmpty() || mqttPassword.isEmpty())
    {
        webserver.send(400, "text/plain", "Formatação dos parâmetros Wi-Fi inválida.");
        return;
    }
    if (!mqttServer.isEmpty() && mqttPort > 0 && mqttPort <= 65535) {
        saveMQTTConfig(mqttServer, mqttPort, mqttUsername, mqttPassword);
        webserver.send(200, "text/plain", "Configurações MQTT salvas com sucesso");
        taskWait(100);
        ESP.restart();
        return;
    }
    webserver.send(400, "text/plain", "Parâmetros MQTT inválidos");
}

void handleGetWifiConfig()
{
    loadWiFiConfig();
    String ssid = wiFiConfig["ssid"].as<String>();
    String psw = wiFiConfig["psw"].as<String>();
    String apSSID = wiFiConfig["apSSID"].as<String>();
    String apPsw = wiFiConfig["apPassword"].as<String>();

    String response = "{\"ssid\":\"" + ssid + "\",\"psw\":\"" + psw + "\",\"apSSID\":\"" + apSSID + "\",\"apPsw\":\"" + apPsw + "\"}";
    webserver.send(200, "application/json", response);
}

void handleSetWifiConfig()
{
    String mqttSettings = webserver.arg("plain");
    if (mqttSettings == "")
    {
        webserver.send(400, "text/plain", "Formatação dos parâmetros MQTT inválida.");
        return;
    }
    if (deserializeJson(mqttConfig, mqttSettings))
    {
        webserver.send(500, "text/plain", "Failed to receive parameters.");
        return;
    }
    String ssid = wiFiConfig["ssid"];
    String psw = wiFiConfig["psw"];
    String apSSID = wiFiConfig["apSSID"];
    String apPsw = wiFiConfig["apPassword"];
    ESP_LOGI("SETTING WIFI", "SSID: %s, PASSWORD: %s", ssid, psw);
    ESP_LOGI("SETTING AP", "AP SSID: %s, AP PASSWORD: %s", apSSID, apPsw);

    // Validar e salvar as configurações Wi-Fi
    if (!ssid.isEmpty() && !psw.isEmpty() && !apSSID.isEmpty() && !apPsw.isEmpty()) {
        saveWiFiConfig(ssid, psw, apSSID, apPsw);
        webserver.send(200, "text/plain", "Configurações Wi-Fi salvas com sucesso");
        taskWait(100);
        return ESP.restart();
    }
    webserver.send(400, "text/plain", "Parâmetros Wi-Fi inválidos");
}

void handleGetAPIConfig()
{
    loadAPIConfig();
    String apiKey = apiConfig["apiKey"].as<String>();
    String phoneNumber = apiConfig["phoneNumber"].as<String>();
    String response = "{\"apiKey\":\"" + apiKey + "\",\"phoneNumber\":\"" + phoneNumber + "\"}";
}

void handleSetAPIConfig()
{
    String apiSettings = webserver.arg("plain");
    if (apiSettings == "")
    {
        webserver.send(400, "text/plain", "Formatação dos parâmetros API inválida.");
        return;
    }
    if (deserializeJson(apiConfig, apiSettings))
    {
        webserver.send(500, "text/plain", "Failed to receive parameters.");
        return;
    }
    String apiKey = apiConfig["apiKey"];
    String phoneNumber = apiConfig["phoneNumber"];
    ESP_LOGI("SETTING API", "API KEY: %s, PHONE NUMBER: %s", apiKey, phoneNumber);

    bool nullSettings = apiKey.isEmpty() || phoneNumber.isEmpty();
    bool invalidParameters = apiKey.length() != 7 || phoneNumber.length() != 12;
    if (nullSettings || invalidParameters)
    {
        webserver.send(400, "text/plain", "Formatação dos parâmetros API inválida.");
        return;
    }
    if (!isAPIKeyValid(apiKey, phoneNumber))
    {
        webserver.send(400, "text/plain", "Chave API ou número inválidos. \nVisite https://shorturl.at/uAWY4 para criar sua Chave API.");
        return;
    }
    saveAPIConfig(apiKey, phoneNumber);
    webserver.send(200, "text/plain", "Configurações API salvas com sucesso.");
    taskWait(1000);
    return ESP.restart();
}


/* Protocolos HTTP */

bool isAPIKeyValid(String key, String phoneNumber)
{
    String message = "Verificação Bem-Sucedida. Este número receberá atualizações do dispositivo MonBot a partir de agora!";
    String url = "https://api.callmebot.com/whatsapp.php?phone="+ phoneNumber +"&text="+ urlEncode(message) +"&apikey="+ key;
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int responseCode = http.POST(url);
    http.end();
    switch(responseCode)
    {
        case HTTP_CODE_OK:
            ESP_LOGI("API Validation", "Valid parameters.");
            return true;
        case HTTP_CODE_NON_AUTHORITATIVE_INFORMATION:
            ESP_LOGE("API Validation", "API Key or Phone number is invalid.");
            return false;
        default:
            ESP_LOGE("API Validation", "Unexpected response code: %u", responseCode);
            return false;
    }
}

void sendAlert(String message)
{
    if (!isConnectedToInternet)
    {
        return;
    }
    if (phoneNumber == "" || apiKey == "")
    {
        ESP_LOGI("ALERT", "No API Key or phone number found");
        return;
    }
    message = "*" + printLocalTime(0)+ "* \n" + message;
    String url = "https://api.callmebot.com/whatsapp.php?phone="+ phoneNumber +"&text="+ urlEncode(message)+"&apikey="+ apiKey;
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int responseCode = http.POST(url);
    http.end();
    switch(responseCode)
    {
        case HTTP_CODE_OK:
            ESP_LOGI("ALERT", "Message sent successfully");
            break;
        case HTTP_CODE_NON_AUTHORITATIVE_INFORMATION:
            ESP_LOGE("API Validation", "API Key or Phone number is invalid.");
            break;
        default:
            ESP_LOGE("API Validation", "Unexpected response code: %u", responseCode);
            break;
    }
}

void configDeviceTime()
{
    if (!isConnectedToInternet)
    {
        return;
    }
    configTime(-3*3600, 3600, ntpServer);
    ESP_LOGI("EPOCH", "Waiting for time configuration...");
    while (!time(nullptr))
    {
        taskWait(1000);
    }
    ESP_LOGI("EPOCH", "Time configured");
    printLocalTime(1);

}

void reconfigTime()
{
    if (!isConnectedToInternet)
    {
        return;
    }
    struct tm timeinfo;
    while (!getLocalTime(&timeinfo))
    {
        ESP_LOGE("EPOCH", "Failed to obtain time");
        taskWait(1000);
    }
    if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0)
    {
        configDeviceTime();
    }

}

String printLocalTime(bool echo=0)
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        ESP_LOGE("EPOCH", "Failed to obtain time");
        return "-1";
    }
    char timeBuffer[80];
    strftime(timeBuffer, sizeof(timeBuffer), "%A, %B %d, %Y. [%H:%M:%S]", &timeinfo);
    if (echo)
    {
        ESP_LOGE("EPOCH", "%s", timeBuffer);
        return "";
    }

    return String(timeBuffer);
}

bool goodMorningTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        return false;
    }
    return (timeinfo.tm_hour == 5 && timeinfo.tm_min == 30);
}