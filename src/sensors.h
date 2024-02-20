#include <DHT.h>

/* DHT */
#define DHTPIN 4
#define DHTTYPE DHT11

float_t verifyTemperature();
float_t verifyHumidity();

DHT dht(DHTPIN, DHTTYPE);

/* Gas Sensor */
#define DO_PIN 34
#define HIGH_GAS_VALUE 1800

void warmUpGasSensor();
bool alertGasSensor();

const char *topicT = "/temperature";
const char *topicH = "/humidity";
const char *topicG = "/gas";

float_t verifyTemperature()
{
  return dht.readTemperature(false);
}

float_t verifyHumidity()
{
  return dht.readHumidity();
}

void warmUpGasSensor()
{
  pinMode(DO_PIN, INPUT);
  // ESP_LOGI("Gas Sensor", "Warming up MQ-2 Sensor");
  ESP_LOGI("Gas Sensor", "Warming up MQ-2 Sensor");
  delay(20000);
  ESP_LOGI("Gas Sensor", "MQ-2 Sensor Ready!");
}

bool gasAlert()
{
  return (analogRead(DO_PIN) >= HIGH_GAS_VALUE);
}
