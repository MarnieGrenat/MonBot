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
void sendAlert(String message);
void configDeviceTime();
String printLocalTime(bool echo);

const char* phoneNumber = "555182680616";
const char* apiKey = "7394641";
const char* ntpServer = "pool.ntp.org";

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
    uint16_t mqttPort = webserver.arg("mqttPort").toInt();
    String mqttUsername = mqttConfig["mqttUsername"].as<String>();
    String mqttPassword = mqttConfig["mqttPassword"].as<String>();

    String response = "{\"mqttServer\":\"" + mqttServer + "\",\"mqttPort\":" + String(mqttPort) + ",\"mqttUsername\":\"" + mqttUsername + "\",\"mqttPassword\":\"" + mqttPassword + "\"}";
    webserver.send(200, "application/json", response);

}

void handleSetMqttConfig() {
    String mqttServer = webserver.arg("mqttServer");
    int mqttPort = webserver.arg("mqttPort").toInt();
    String mqttUsername = webserver.arg("mqttUser");
    String mqttPassword = webserver.arg("mqttPsw");

    // Validar e salvar as configurações MQTT
    if (!mqttServer.isEmpty() && mqttPort > 0 && mqttPort <= 65535) {
        saveMQTTConfig(mqttServer, mqttPort, mqttUsername, mqttPassword);
        webserver.send(200, "text/plain", "Configurações MQTT salvas com sucesso");
        delay(100);
        ESP.restart();
    } else {
        webserver.send(400, "text/plain", "Parâmetros MQTT inválidos");
    }
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

void handleSetWifiConfig() {
    String ssid = webserver.arg("ssid");
    String password = webserver.arg("psw");
    String apSSID = webserver.arg("apSSID");
    String apPassword = webserver.arg("apPsw");

    // Validar e salvar as configurações Wi-Fi
    if (!ssid.isEmpty() && !password.isEmpty() && !apSSID.isEmpty() && !apPassword.isEmpty()) {
        saveWiFiConfig(ssid, password, apSSID, apPassword);
        webserver.send(200, "text/plain", "Configurações Wi-Fi salvas com sucesso");
        delay(100);
        ESP.restart();
    } else {
        webserver.send(400, "text/plain", "Parâmetros Wi-Fi inválidos");
    }
}


/* Protocolos HTTP */

void sendAlert(String message)
{
    if (!isConnectedToInternet)
    {
        return;
    }
    message = "*" + printLocalTime(0)+ "* \n" + message;
    String url = "https://api.callmebot.com/whatsapp.php?phone="+ String(phoneNumber) +"&text="+ urlEncode(message)+"&apikey="+ String(apiKey);
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int responseCode = http.POST(url);
    if (responseCode == HTTP_CODE_OK)
    {
        Serial.print("Message sent successfully");
        http.end();
        return;
    }
    Serial.println("Falha ao enviar mensagem. Código de resposta:");
    Serial.println(responseCode);
    http.end();
}

void configDeviceTime()
{
    if (!isConnectedToInternet)
    {
        return;
    }
    configTime(-3*3600, 3600, ntpServer);
    Serial.println("Waiting for time");
    while (!time(nullptr))
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("Time configured");
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
        Serial.println("Failed to obtain time");
        delay(1000);
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
        Serial.println("Failed to obtain time");
        return "-1";
    }
    if (echo)
    {
        Serial.println(&timeinfo, "%A, %B / %d / %Y. [%H:%M:%S]");
        return "";
    }
    // Serial.println("LocalTime sent Successfully");

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%A, %B %d, %Y. [%H:%M:%S]", &timeinfo);
    return String(buffer);
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