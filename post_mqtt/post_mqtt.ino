#define DEBUG

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>

#include <MAX44009.h>
MAX44009 light;


#define SIZE 90
const char *ssid = "Robolab";   // name of your WiFi network
const char *password = "W3<3R0bots"; // password of the WiFi network

char* mqttUser = (char*) malloc(395);
const char* mqttPassword = "parola_smechera";
const char* mqttServer = "52.47.169.152";
const int mqttPort = 1883;


const char *ID = "Example_bme680_2";  // Name of our device, must be unique
const char *TOPIC_TEMP = "1";  // Topic to subcribe to
const char *TOPIC_PRESSURE = "2";
const char *TOPIC_HUM = "3";
const char *TOPIC_GAS = "4";
const char *TOPIC_LIGHT = "5";

IPAddress broker(35, 181, 26, 170); // IP address of your MQTT broker eg. 192.168.1.50
WiFiClient wclient;
PubSubClient client(wclient); // Setup MQTT client


// I2C sensors
float lux = 0.0;

// timer variable
unsigned long elapsed_msec;


// Connect to WiFi network
void setup_wifi() {
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // Connect to network

  while (WiFi.status() != WL_CONNECTED) { // Wait for connection
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


// Reconnect to client
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ID)) {
      client.subscribe(TOPIC_TEMP);
      Serial.println("connected");
      Serial.print("Subcribed to: ");
      Serial.println(TOPIC_TEMP);
      Serial.println('\n');

    } else {
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


String make_payload(String hash, String value) {
  String payload;
  payload = "{";
  payload += "\n";
  payload += "\"hash\": \"416310F139FB5BF07790D01D4B1CE4C1C88FCDFB\",";
  payload += "\n";
  payload += "\"value\": ";
  payload += value;
  payload += "\n";
  payload += "}";
  return payload;
}


void setup()
{
#ifdef DEBUG
  elapsed_msec = millis();
#endif


  Serial.begin(9600);  // Init serial link for debugging

  setup_wifi(); // Connect to network
  memcpy(mqttUser, "eyJhbGciOiJSUzUxMiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIwIn0.F9bZi7kjz4NN3i_tvpAaGppV-jdxtRw3THsY9oG-8stZXrKlburQ5AvUpBEQm7fkSppZT-_ICYn2kPAETVATXCfe9qz-Mx4i2uSsvLWQBESwpuUgzm_4HqagD_pDnpM6w6l8y9fcj55sOnew08VEgYGzilF0sIdIlNImkKlRr4_NITG0rjtos7ROddxwbi1SIb0Aj6gZODbmiPx7srEs-m0aTcGuoBtW7TbQGGT7jhHjLr7bDCGio9Ys8ogxldf-y2tDXqRV9qjWxuNLGF6NUiR58OI-mqeIHMZFU-1k3PgY-dwRIeR5u4dO5DOJeoZL0-VCqWEP9tPCUU2aS0fkxQ", 395);
  
  client.setServer(mqttServer, mqttPort);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP smecher", mqttUser, mqttPassword )) {
      //    if (client.connect(ID)) {

      Serial.println("connected");

    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);

    }
  }


  //initializam senzorii
  Wire.begin();

  if (light.begin())
  {
    Serial.println("Could not find a valid MAX44009 sensor, check wiring!");
    while (1);
  }


}

void loop()
{
  lux = light.get_lux();

#ifdef DEBUG
  Serial.print("Light = ");
  Serial.print(lux);
  Serial.println(" lux");
#endif

  //#ifdef DEBUG
  //  Serial.print("Millis elapsed: ");
  //  Serial.println(millis() - elapsed_msec);
  //#endif

  //pregatim datele de trimis
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();

  JSONencoder["device"] = "ESP8266";
  JSONencoder["sensorType"] = "Light";
  JSONencoder["Value"] = lux;

  char JSONmessageBuffer[100];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println("Sending message to MQTT topic..");
  Serial.println(JSONmessageBuffer);

  if (client.publish("0/0", JSONmessageBuffer) == true) {
    Serial.println("Success sending message");
  } else {
    Serial.println("Error sending message");
  }

  client.loop();
  Serial.println("-------------");

  delay(10000);

  //  // sending data
  //  reconnect();
  //  client.loop();
  //  String payload_light = make_payload("light", String(lux));
  //  char text[50];
  //  payload_light.toCharArray(text, SIZE);
  //  Serial.println(text);
  //  client.publish(TOPIC_LIGHT, text);
  //  client.disconnect();

  //  for sleeping and power save energy
  //  ESP.deepSleep(5000000, WAKE_RF_DEFAULT); //sleep for 5 seconds
}


