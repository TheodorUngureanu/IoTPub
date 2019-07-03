#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <MAX44009.h>

#define DEBUG
#define SEALEVELPRESSURE_HPA (1013.25)


// I2C sensors
MAX44009 light;
float lux = 0.0, temperature, pressure, humidity, voc, altitude;
Adafruit_BME680 bme; // I2C

//wifi connect
int connected = 0;

// mqtt authentification
char* mqttUser = (char*) malloc(395);
const char* mqttPassword = "parola_smechera";
const char* mqttServer = "35.180.97.186";
const int mqttPort = 1883;

//IPAddress broker(35, 180, 208, 18); // IP address of your MQTT broker eg. 192.168.1.50
WiFiClient wclient;
PubSubClient client(wclient); // Setup MQTT client


//SSID and Password to your ESP Access Point
const char* ssid = "IoTPub New Device";
const char* password = "bumbumsacalaca";

ESP8266WebServer server(80); //Server on port 80


void handleRoot() {
  //pregatim datele de trimis
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();

  int n = WiFi.scanNetworks();
  Serial.println("scan done");

  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
      //      JSONencoder[String(i)] = String(WiFi.SSID(i) + " " + WiFi.RSSI(i));
      JSONencoder[WiFi.SSID(i)] = WiFi.RSSI(i);
    }
  }

  //  trimiterea datelor
  char JSONmessageBuffer[400];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println("Sending Wifi networks");
  Serial.println(JSONmessageBuffer);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", JSONmessageBuffer);
}


void authRoutine() {

  server.sendHeader("Access-Control-Allow-Origin", "*");
  //  server.sendHeader("Retry-After", "15");
  //  server.sendHeader("Status", "469");
  //  server.send(200, "text/plain", "trying conection...");

  String ssid_name;
  String psswd;

  Serial.println("Cerere de auth");
  Serial.print("Numarul de argumente este:");
  Serial.println(server.args());

  if (server.args() == 2 ) { // Arguments were received
    if ((server.argName(0) == "SSID" && server.argName(1) == "PSSWD") ||
        (server.argName(0) == "ssid" && server.argName(1) == "psswd")) {

      Serial.println("Am primit doua argumente valide");
      Serial.println("SSID: " + server.arg(0));
      Serial.println("PSSWD: " + server.arg(1));
      ssid_name = server.arg(0);
      psswd = server.arg(1);

      //  const char* ssid = "Robolab";
      //  const char* password = "W3<3R0bots";

      //  Convert String to char*
      int psswd_len = psswd.length() + 1;
      int ssid_len = ssid_name.length() + 1;

      char psswd_array[psswd_len];
      char ssid_array[ssid_len];

      ssid_name.toCharArray(ssid_array, ssid_len);
      psswd.toCharArray(psswd_array, psswd_len);

      //  WiFi.begin(ssid, password);
      WiFi.begin(ssid_array, psswd_array);

      int ok = 0;
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        ok++;
        Serial.print(".");
        if (ok == 20) {
          //          server.sendHeader("Access-Control-Allow-Origin", "*");
          server.send(298, "text/plain", "Conection to Wi-Fi failed");
          Serial.println("Conection to Wi-Fi failed");
          return;
        }
      }

      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());

      //      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.sendHeader("Status", "200");
      server.send(200, "text/plain", "Conected");
      //      mqtt_setup();
      connected = 1;
      //    server.close();
    }
    else {
      Serial.println("Wrong arguments names");
      server.sendHeader("Status", "200");
      server.send(299, "text/plain", "Wrong arguments names");
    }
  }

  else {
    //    server.sendHeader("Access-Control-Allow-Origin", "*");
    Serial.println("Wrong number of arguments :(");
    server.sendHeader("Status", "200");
    server.send(299, "text/plain", "Wrong number of arguments :(");
  }
}


// mqtt setup
void mqtt_setup() {
  memcpy(mqttUser, "eyJhbGciOiJSUzUxMiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxIn0.Lt3acoDXi86Tv3yMSpP2H-bpjKxoN7WNXZgQKuuiiIPUfCMxW1KDN3QtVpbJtqZfCoKftDicoLTSellsngVYgdr0udT5byWx9XLqElB-WrGbYAgIRvVxhaWZiR1KYl0tqcaWxBILOg1jcvZJLaSmbpXI08Urgkt8H0UaI2QpcuScQ27E25f58ojeelZnXzBFyYBj-HgUGdx0_gJRfxTKPsv01PryLeEWA-f7e_sUpJ-GeXhsu1gFSqMRRX0VWEPmU1HyY8JEwCMOp0bnxoRRdhjQRKwLamubRQm08abz8FtGBOR7wWjCPQUa7KSzvJIyxWJTvZebOq3RHwR4aCwFww", 395);

  client.setServer(mqttServer, mqttPort);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP smecher", mqttUser, mqttPassword )) {
      Serial.println("connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}


//setupul general
void setup(void) {
  Serial.begin(9600);
  Serial.println("Serial working");

  WiFi.mode(WIFI_AP);           //Only Access point
  WiFi.softAP(ssid, password);  //Start HOTspot removing password will disable security

  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print("HotSpt IP:");
  Serial.println(myIP);

  server.on("/", handleRoot);      //Which routine to handle at root location
  server.on("/auth", authRoutine);

  server.begin();                  //Start server
  Serial.println("HTTP server started");

  //initializam senzorii
  Wire.begin();

  // pentru senzorul de lumina
  if (light.begin())
  {
    Serial.println("Could not find a valid MAX44009 sensor, check wiring!");
    while (1);
  }

  //  pentru senzorul bme680
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

void send_data(String name, float value, char* topic) {
  //pregatim datele de trimis
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();

  //    pentru senzor
  JSONencoder["value"] = value;
  char JSONmessageBuffer[100];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println("Sending message to MQTT topic..");
  Serial.println(JSONmessageBuffer);
  if (client.publish(topic, JSONmessageBuffer) == true) {
    Serial.println("Success sending message");
  } else {
    Serial.println("Error sending message");
  }

}

void loop(void) {

  if (!connected) {
    server.handleClient();          //Handle client requests
  }

  else {
    mqtt_setup();

    //    citim senzorii
    if (! bme.performReading()) {
      Serial.println("Failed to perform reading :(");
      return;
    }
    if (! bme.performReading()) {
      Serial.println("Failed to perform reading :(");
      return;
    }


    lux = light.get_lux();
    temperature = bme.temperature;
    pressure = bme.pressure / 100.0;
    humidity = bme.humidity;
    voc = bme.gas_resistance / 1000.0;
    altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);


#ifdef DEBUG
    Serial.print("Light ............. ");
    Serial.print(lux);
    Serial.println(" lux");

    Serial.print("Temperature ....... ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Pressure .......... ");
    Serial.print(pressure);
    Serial.println(" hPa");

    Serial.print("Humidity .......... ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.print("Gas ............... ");
    Serial.print(voc);
    Serial.println(" KOhms");

    Serial.print("Approx. Altitude .. ");
    Serial.print(altitude);
    Serial.println(" m");
#endif

    //#ifdef DEBUG
    //  Serial.print("Millis elapsed: ");
    //  Serial.println(millis() - elapsed_msec);
    //#endif

    send_data("light", lux, "t_ungureanu/1");
    send_data("temperature", temperature, "t_ungureanu/2");
    send_data("pressure", pressure, "t_ungureanu/3");
    send_data("humidity", humidity, "t_ungureanu/4");
    send_data("voc", voc, "t_ungureanu/5");
    send_data("altitude", altitude, "t_ungureanu/6");


    //    client.loop();
    Serial.println("-------------");
    //    trimite la 5 minute
    //    delay(300000);

    //    trimite la 10 s
    delay(10000);
  }
}


