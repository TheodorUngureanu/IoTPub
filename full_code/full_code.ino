#define SEALEVELPRESSURE_HPA (1013.25)

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <MAX44009.h>

// for luxmeter sensor
MAX44009 light;


// I2C sensors
Adafruit_BME680 bme;

float temperature, pressure, humidity, voc, lux;

// timer variable
unsigned long elapsed_msec;


// initializare bme
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

  //  pinul pentru modul de access point
  pinMode(13, OUTPUT);

  //  WiFiManager wifiManager;
  //  wifiManager.startConfigPortal("ESP 8266");
  //  Serial.println("connected...yeey :)");

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // Uncomment and run it once, if you want to erase all the stored information
  //  wifiManager.resetSettings();

  // incearca sa se conecteze la retelele din memoria eprom si daca nu reuseste isi face access point
  //  wifiManager.autoConnect("ESP8266 AP");

  if ( digitalRead(13) == HIGH)
  {
    wifiManager.startConfigPortal("ESP8266 AP");
    //    optional stergem si istoricul
    //    wifiManager.resetSettings();
  }
  else {
    wifiManager.autoConnect("ESP8266 Brand New");

  }

  // if you get here you have connected to the WiFi
  Serial.println("Connected.");


  // initializare sensor de lumina
  if (!light.begin())
  {
    Serial.println("Could not find a valid MAX44009 sensor, check wiring!");
  }

  if (!BME680init())
  {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    ESP.deepSleep(10000000, WAKE_RF_DEFAULT); // go to sleep for 10s to force a reset
  }

  //  citire date de la bme
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
  lux = light.get_lux();

  Serial.print("Temperature ...... ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Pressure ......... ");
  Serial.print(pressure);
  Serial.println(" hPa");

  Serial.print("Humidity ......... ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Gas .............. ");
  Serial.print(voc);
  Serial.println(" KOhms");

  Serial.print("Light ............ ");
  Serial.print(lux);
  Serial.println(" lux");

  Serial.print("Esp ip adresss: ");
  Serial.println(WiFi.localIP());

  //sleep for 10 seconds
  ESP.deepSleep(10000000, WAKE_RF_DEFAULT);
}

void loop()
{

}
