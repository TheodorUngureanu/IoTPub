#include <EEPROM.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <MAX44009.h>

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ATT_IOT.h>

#define DEVICE_ID "wc2ZCnXfSB7zAuv7uaJVN14V"
#define DEVICE_TOKEN "maker:4Krzy0w46ZKyy05zGzwweepJ2c0XsF25Toub2jRF"

// Define http and mqtt endpoints
#define HTTP_URL "api.allthingstalk.io"  // API endpoint
#define MQTT_URL "api.allthingstalk.io"  // MQTT broker

WiFiClient espClient;
PubSubClient pubSub(MQTT_URL, 1883, 0, espClient);
ATTDevice device(DEVICE_ID, DEVICE_TOKEN);

#include <CborBuilder.h>
CborBuilder payload(device);

#define SEALEVELPRESSURE_HPA (1013.25)

// for luxmeter sensor
MAX44009 light;

// I2C sensors
Adafruit_BME680 bme;

float temperature, pressure, humidity, voc, lux, altitude;

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
  Wire.begin();   // foarte important

  //  structura pentru salvarea tokenului in eprom
  uint addr = 0;
  struct {
    uint val = 0;
    char token[20] = "";
  } data;

  // pentru scrierea in EEPROM
  EEPROM.begin(512);
  EEPROM.get(addr, data);
  Serial.println();
  Serial.println("Old values are: " + String(data.token) + " & " + String(data.val));

  //  aici vom adauga tokenul pe care trebuie sa il scriem in memorie
  if (data.val != 1) {
    Serial.println("modificam in memorie");
    strncpy(data.token, "password", 20);
    data.val = 1;
    // replace values in byte-array cache with modified data
    // no changes made to flash, all in local byte-array cache
    EEPROM.put(addr, data);
    EEPROM.commit();
  }

  //  pinul pentru modul de access point
  pinMode(13, OUTPUT);

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // Uncomment and run it once, if you want to erase all the stored information
  //  wifiManager.resetSettings();

  // incearca sa se conecteze la retelele din memoria eprom si daca nu reuseste isi face access point
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
  if (light.begin())
  {
    Serial.println("Could not find a valid MAX44009 sensor, check wiring!");
  }

  if (!BME680init())
  {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    ESP.deepSleep(10000000, WAKE_RF_DEFAULT); // go to sleep for 10s to force a reset
  }

  while (!device.subscribe(pubSub)) // Subscribe to mqtt
    Serial.println("retrying");

  //  citire date de la bme
  if (!bme.performReading())
  {
    Serial.println("[BME680] Failed to perform reading :(");
    return;
  }
  if (!bme.performReading())
  {
    Serial.println("[BME680] Failed to perform reading :(");
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

  //  EEPROM ce avem in memorie
  EEPROM.get(addr, data);
  Serial.println("New values are: " + String(data.token) + " & " + String(data.val));
}


void loop()
{

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
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

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

  Serial.print("Approx. Altitude .. ");
  Serial.print(altitude);
  Serial.println(" m");

  // trimitem datele catre server
  payload.reset();
  payload.map(6); //send a total of 5 values for the declared assets
  payload.addNumber(temperature, "temperature");
  payload.addNumber(pressure, "pressure");
  payload.addNumber(humidity, "humidity");
  payload.addNumber(voc, "voc");
  payload.addNumber(lux, "light");
  payload.addNumber(lux, "altitude");
  payload.send();


  //low power
  //every 15 min
  //  delay(900000);

  Serial.println("Sleep 10 seconds");
  delay(10000);

  //sleep for 10 seconds
  //  ESP.deepSleep(10000000, WAKE_RF_DEFAULT);
}
