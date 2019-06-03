#define SEALEVELPRESSURE_HPA (1013.25)

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

ESP8266WebServer Server;
AutoConnect      Portal(Server);

#include <AutoConnectCredential.h>

void rootPage() {
  char content[] = "Hello, world";
  Server.send(200, "text/plain", content);
}

// I2C sensors
Adafruit_BME680 bme;

float temperature, pressure, humidity, voc;

// timer variable
unsigned long elapsed_msec;

int BME680init()
{

  if (!bme.begin())
    return 0;

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  return 1;
}



void setup()
{
  Serial.begin(9600);  // Init serial link for debugging

  if (!BME680init())
  {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    ESP.deepSleep(10000000, WAKE_RF_DEFAULT); // go to sleep for 10s to force a reset
  }

  // connect to wifi
  Server.on("/", rootPage);
  if (Portal.begin()) {
    Serial.println("HTTP server:" + WiFi.localIP().toString());
  }

  if (!bme.performReading())
  {
    Serial.println("Failed to perform reading :(");
    return;
  }
  if (!bme.performReading())
  {
    Serial.println("Failed to perform reading :(");
    return;
  }

  temperature = bme.temperature;
  pressure = bme.pressure / 100.0;
  humidity = bme.humidity;
  voc = bme.gas_resistance / 1000.0;

  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(pressure);
  Serial.println(" hPa");

  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Gas = ");
  Serial.print(voc);
  Serial.println(" KOhms");

  Serial.println(WiFi.localIP());

  //sleep for 10 seconds
  //  ESP.deepSleep(10000000, WAKE_RF_DEFAULT);


}

void loop()
{

}
