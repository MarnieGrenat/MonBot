#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <MQTT.h>
#include "globals.h"
#include "http_protocol.h"
#include "sensors.h"

/* Function Prototypes */
void initSerial(uint32_t baudRate);
void presentDevice();
void initWifi();
void taskMaintainWifi(void*);
void updateNetworkInfo();
void initAccessPoint(const char* ssid, const char* psw);

void taskMQTT(void*);
String mqttError(lwmqtt_err_t error);
void sendMQTTMessage(String topic, String payload, uint8_t qos = 0);
void treatReceivedMessage(String &topic, String &payload);


void taskControl(void*);

/* Variables */
static MQTTClient *mqttClient;
static WiFiClientSecure *wifiClientSecure;
static WiFiClient *wifiClientUnsecured;

/* Task Handles */
TaskHandle_t taskHandle_WiFi;
TaskHandle_t taskHandle_Control;
TaskHandle_t taskHandle_MQTT;

void setup() {
  while (!SPIFFS.begin(true))
  {
    taskWait(1000);
  }
  ESP_LOGI("SETUP", "INIT SETUP.");
  initSerial(115200);
  taskWait(1000);
  Serial.flush();
  presentDevice();
  loadAPIConfig();
  initWifi();

  configDeviceTime();

  ESP_LOGI("SETUP", "Webserver initialized. IP Address: ");
  ESP_LOGI("SETUP", "%s", WiFi.localIP());
  ESP_LOGI("SETUP", "Setup complete");
  taskWait(1000);
  xTaskCreate(taskControl, "control", 10 * 1024, NULL, 2, &taskHandle_Control);
  taskWait(10*1000);
  sendAlert("Iniciando Monitoramento do Sistema.");
  warmUpGasSensor();
}

void loop() {
  webserver.handleClient();
  taskWait(500);
}

/* Serial */
void initSerial(uint32_t baudRate)
{
  Serial.begin(baudRate);
  ESP_LOGI("SETUP", "Serial initialized");
}

void presentDevice()
{
  ESP_LOGI("SETUP", "************************************************");
  ESP_LOGI("SETUP", "SPIFFS Access Initialized.");
  ESP_LOGI("SETUP", "Initializing Device...");
  // ESP_LOGI("SETUP", "Device: %s Version: %s" String(DEVICE), String(VERSION));
  ESP_LOGI("SETUP", "************************************************");
}
/* Control */
void taskControl(void*)
{
  time_t lastMessage = time(nullptr);
  for(;;)
  {
    if (!isConnectedToInternet)
    {
      taskWait(1000);
      continue;
    }
    if (goodMorningTime())
    {
      reconfigTime();

      String messageInfo = "\nTemperatura: " + String(verifyTemperature()) + "C\nUmidade: " + String(verifyHumidity()) + "%";
      sendAlert("Bom dia!" + messageInfo + "\nTenha um ótimo dia hoje também!");
    }
    if (alertGasSensor())
    {
      if (lastMessage - time(nullptr) < 60 * 30)
      {
        taskWait(20 * 1000);
        continue;
      }
      lastMessage = time(nullptr);
      const char* gasAlert = "Atenção! Alerta!\nGás inflamável Detectado!\nNão acenda luzes ao entrar no ambiente!\nVentile o ambiente e saia imediatamente!\nDicas: https://encurtador.com.br/bsCKL";
      sendAlert(gasAlert);
      sendMQTTMessage("Alerta", "Alerta de Gás Detectado!", 1);
    }
    taskWait(20 * 1000);
  }
  vTaskDelete(NULL);
}
/* WIFI */

void initWifi()
{
  WiFi.setSleep(0);
  WiFi.setAutoReconnect(0);
  loadWiFiConfig();
  taskWait(100);
  const char *apSSID = wiFiConfig["apSSID"];
  const char *apPassword = wiFiConfig["apPassword"];
  initAccessPoint(apSSID, apPassword);

  const char *ssid = wiFiConfig["ssid"];
  const char *password = wiFiConfig["psw"];

  if (password == "")
  {
    WiFi.begin(ssid);
  }
  if (ssid == "")
  {
    taskWait(10000);
  }
  WiFi.begin(ssid, password);

  ESP_LOGI("Wi-Fi", "Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED)
  {
    taskWait(1000);
  }
  xTaskCreate(taskMaintainWifi, "maintain WIFI", 4 * 1024, NULL, 2, &taskHandle_WiFi);
  taskWait(1000);
  xTaskCreate(taskMQTT, "connect to MQTT", 10 * 1024, NULL, 2, &taskHandle_MQTT);

  taskWait(10000);
  sendMQTTMessage("Status", "Dispositivo Conectado.", 1);
}

void taskMaintainWifi(void *)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    updateNetworkInfo();
    ESP_LOGI("Wi-Fi", "\nConnected to WiFi: %s", WiFi.SSID());
    sendAlert("Conectado a rede WiFi: " + WiFi.SSID());
  }
  for(;;)
  {
    isConnectedToInternet = (WiFi.status() == WL_CONNECTED);
    if (!isConnectedToInternet)
    {
      ESP_LOGI("Wi-Fi", "Connection lost. Reconnecting...");
      WiFi.reconnect();
      configDeviceTime();
    }
    taskWait(10 * 1000);
  }
  vTaskDelete(NULL);
}

void updateNetworkInfo()
{
  IPAddress ip = WiFi.localIP();
  ESP_LOGI("Wi-Fi", "IP Address: %s Gateway: %s Subnet Mask: %s", ip.toString(), WiFi.gatewayIP().toString(), WiFi.subnetMask().toString());
}

void initAccessPoint(const char *ssid, const char *password)
{

  ESP_LOGI("Wi-Fi", "%s, %s", ssid, password);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 0), IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password, 1, 0, 2);
  initWebserver();

  ESP_LOGI("Access Point", "Webserver Initialized");
  ESP_LOGI("Access Point", "Access IP: %s", WiFi.softAPIP().toString());
  sendAlert("Ponto de Acesso Inicializado. SSID: " + String(ssid) + "\nIP: " + WiFi.softAPIP().toString());
}

void taskMQTT(void*)
{
  loadMQTTConfig();
  const char* mqttServer = mqttConfig["mqttServer"].as<const char*>(); // Use const char* for consistency
  const uint16_t mqttPort = mqttConfig["mqttPort"].as<uint16_t>();
  const char* mqttUser = mqttConfig["mqttUser"].as<const char*>();
  const char* mqttPassword = mqttConfig["mqttPsw"].as<const char*>();
  while (mqttServer == "")
  {
    taskWait(1000);
  }

  mqttClient = new MQTTClient(8 * 1024);
  mqttClient->setKeepAlive(55);
  mqttClient->setCleanSession(1);

  WiFiClient* wifiClient;
  if (mqttPort == 1883)
  {
    wifiClient = wifiClientUnsecured = new WiFiClient;
  }
  else
  {
    wifiClient = wifiClientSecure = new WiFiClientSecure;
    wifiClientSecure->setInsecure();
  }

  mqttClient->begin(mqttServer, mqttPort, *wifiClient);
  mqttClient->onMessage(treatReceivedMessage);

  sendAlert("Conectado ao Broker MQTT.");

  for (;;)
  {
    isConnectedToMQTT = mqttClient->connected();
    if (!isConnectedToMQTT)
    {
      ESP_LOGI("MQTT", "Connection to Broker lost. Reconnecting...");
      while (!mqttClient->connect("Monitoramento", mqttUser, mqttPassword))
      {
        ESP_LOGE("MQTT", "Failed to reconnect to Broker. Trying again in 10 seconds...");
        ESP_LOGE("MQTT", "Connection Error: %s", mqttError(mqttClient->lastError()));
        mqttClient->loop();
        taskWait(10000);
      }
      ESP_LOGI("MQTT", "Reconnected to Broker.");
    }
    mqttClient->loop();
    taskWait(1000);
  }
  vTaskDelete(NULL);
}

String mqttError(lwmqtt_err_t error)
{
    switch(error)
  {
    case 0:
      return "Connection Accepted";
      break;
    case -1:
      return "Buffer too Short.";
      break;
    case -2:
      return "Varnum Overflow.";
      break;
    case -3:
      return "Network Failed Connect.";
      break;
    case -4:
      return "Network Timeout.";
      break;
    case -5:
      return "Network Failed Read.";
      break;
    case -6:
      return "Network Failed Write.";
      break;
    case -7:
      return "Remaining Length Overflow.";
      break;
    case -8:
      return "Remaining Length Mismatch.";
      break;
    case -9:
      return "Missing or Wrong Packet.";
      break;
    case -10:
      return "Connection Denied.";
      break;
    case -11:
      return "Failed to Subscribe.";
      break;
    case -12:
      return "Suback Array Overflow.";
      break;
    case -13:
      return "Pong Timeout.";
      break;
    default:
      return "Unknown Error.";
      break;
  }
  taskWait(10 * 1000);
}

void sendMQTTMessage(String topic, String payload, uint8_t qos)
{
  if (isConnectedToMQTT)
  {
    mqttClient->publish(topic.c_str(), payload.c_str(), false, qos);
  }
}

void treatReceivedMessage(String &topic, String &payload)
{
  ESP_LOGI("MQTT", "Received message from topic: %s - %s", topic, payload);

  if (topic == "command" && payload == "reboot")
  {
    sendAlert("Comando Recebido. Reiniciando o dispositivo.");
    taskWait(1000);
    ESP.restart();
  }
  else if (topic == "time" && payload == "update")
  {
    ESP_LOGI("MQTT","Comando Recebido. Reconfigurando temporizador.");
    taskWait(1000);
    configDeviceTime();
  }
  else
  {
    sendAlert("Comando MQTT Desconhecido Recebido. Tópico/Payload:" + topic + " / " + payload);
    ESP_LOGE("MQTT", "Tópico não reconhecido.");
  }
}