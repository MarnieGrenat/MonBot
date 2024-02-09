#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <MQTT.h>
#include "globals.h"
#include "http_proto.h"
#include "sensors.h"

/* Function Prototypes */
void initSerial(uint32_t baudRate);

void loadWiFiConfig();
void initWifi();
void taskMaintainWifi(void*);
void updateNetworkInfo();
void initAccessPoint(const char* ssid, const char* psw);
void taskMQTT(void*);
void taskControl(void*);
String mqttError(lwmqtt_err_t error);
void sendMQTTMessage(String topic, String payload, uint8_t qos = 0);
void treatReceivedMessage(String &topic, String &payload);

/* Variables */
static MQTTClient *mqttClient;
static WiFiClientSecure *wifiClientSecure;
static WiFiClient *wifiClientUnsecured;


void setup() {
  while (!SPIFFS.begin(true))
  {
    delay(1000);
  }
  initSerial(115200);
  delay(1000);
  Serial.flush();
  initWifi();

  configDeviceTime();

  Serial.println("Webserver initialized. IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Setup complete");
  delay(1000);
  xTaskCreate(taskMaintainWifi, "maintain WIFI", 4 * 1024, NULL, 2, NULL);
  xTaskCreate(taskControl, "control", 10 * 1024, NULL, 2, NULL);
  delay(10*1000);
  sendAlert("Iniciando Monitoramento do Sistema.");
}

void loop() {
  webserver.handleClient();
  delay(500);
}

/* Serial */
void initSerial(uint32_t baudRate)
{
  Serial.begin(baudRate);
  Serial.println("Serial initialized");
}

/* Control */
void taskControl(void*)
{
  warmUpGasSensor();
  for(;;)
  {
    if (!isConnectedToInternet)
    {
      continue;
    }

    if (goodMorningTime())
    {
      float_t temperature = verifyTemperature();
      float_t humidity = verifyHumidity();
      String message = "Temperatura: " + String(temperature) + "C\nUmidade: " + String(humidity) + "%";
      sendAlert("Bom dia!" + message + "\nTenha um ótimo dia hoje também!");
      delay(10 * 1000);
    }
    if (alertGasSensor())
    {
      sendAlert("Alerta de Gás Detectado!\nNível de Gás: Alto\nAtenção! Não acenda luzes ao entrar no ambiente!\nVentile o ambiente e saia imediatamente!\nhttps://segredosdomundo.r7.com/vazamento-de-gas-o-que-fazer-quando-acontecer/");
      sendMQTTMessage("Alerta", "Alerta de Gás Detectado!", 1);
      delay(30 * 1000);
    }
    taskWait(20 * 100);
  }
  vTaskDelete(NULL);
}
/* WIFI */

void initWifi()
{
  WiFi.setSleep(0);
  WiFi.setAutoReconnect(0);
  const char *ssid = wiFiConfig["ssid"];
  const char *password = wiFiConfig["psw"];
  const char *apSSID = wiFiConfig["apSSID"];
  const char *apPassword = wiFiConfig["apPassword"];

  initAccessPoint(apSSID, apPassword);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi..");
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(10000);
  }
  xTaskCreate(taskMaintainWifi, "maintain WIFI", 4 * 1024, NULL, 2, NULL);
  xTaskCreate(taskMQTT, "connect to MQTT", 10 * 1024, NULL, 2, NULL);

  delay(10000);
  sendMQTTMessage("Status", "Dispositivo Conectado.", 1);
}

void taskMaintainWifi(void *)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    updateNetworkInfo();
    Serial.println("\n\nConnected to WiFi: " + WiFi.SSID());
    sendAlert("Conectado a rede WiFi: " + WiFi.SSID());
  }
  for(;;)
  {
    isConnectedToInternet = (WiFi.status() == WL_CONNECTED);
    if (!isConnectedToInternet)
    {
      Serial.println("WiFi connection lost. Reconnecting...");
      WiFi.reconnect();
      // sendAlert("Conexão WiFi perdida. Reconectando...");
      configDeviceTime();
    }
    delay(10 * 1000);
  }
  vTaskDelete(NULL);
}

void updateNetworkInfo()
{
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: " + ip.toString() + " Gateway: " + WiFi.gatewayIP().toString() + " Subnet Mask: " + WiFi.subnetMask().toString());
}

void initAccessPoint(const char *ssid, const char *password)
{
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(IPAddress(192, 168, 0, 10), IPAddress(192, 168, 0, 10), IPAddress(255, 255, 255, 240));
  WiFi.softAP(ssid, password, 1, 0, 2);
  initWebserver();

  String apSSID = ssid;
  Serial.println("Access Point initialized");
  Serial.println("AP SSID: " + apSSID + " Password: " + password + " IP: " + WiFi.softAPIP().toString());
  sendAlert("Ponto de Acesso Inicializado. SSID: " + apSSID + "\nIP: " + WiFi.softAPIP().toString());
}

void taskMQTT(void*)
{
  loadMQTTConfig();
  const char* mqttServer = mqttConfig["mqttServer"].as<const char*>(); // Use const char* for consistency
  const uint16_t mqttPort = mqttConfig["mqttPort"].as<uint16_t>();
  const char* mqttUser = mqttConfig["mqttUser"].as<const char*>();
  const char* mqttPassword = mqttConfig["mqttPsw"].as<const char*>();

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
      Serial.println("MQTT connection lost. Reconnecting...");
      while (!mqttClient->connect("Monitoramento", mqttUser, mqttPassword))
      {
        Serial.println("Failed to reconnect to MQTT. Trying again in 10 seconds...");
        Serial.print("MQTT Error: " + mqttError(mqttClient->lastError()));
        mqttClient->loop();
        delay(10000);
      }
      Serial.println("Reconnected to MQTT.");
    }
    mqttClient->loop();
    delay(10000);
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
  Serial.println("MQTT Error: " + error);
  delay(10 * 1000);
}

void sendMQTTMessage(String topic, String payload, uint8_t qos)
{
  if (mqttClient->connected())
  {
    mqttClient->publish(topic.c_str(), payload.c_str(), false, qos);
  }
}

void treatReceivedMessage(String &topic, String &payload)
{
  Serial.println("Received message from topic: " + topic + " - " + payload);

  if (topic == "command" && payload == "reboot")
  {
    sendAlert("Comando Recebido. Reiniciando o dispositivo.");
    delay(1000);
    ESP.restart();
  }
  else if (topic == "time" && payload == "update")
  {
    Serial.println("Comando Recebido. Reconfigurando temporizador.");
    delay(1000);
    configDeviceTime();
  }
  else
  {
    sendAlert("Comando MQTT Desconhecido Recebido. Tópico/Payload:" + topic + " / " + payload);
    Serial.println("Tópico não reconhecido.");
  }
}