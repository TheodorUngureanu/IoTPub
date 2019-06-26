#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

WiFiClient espClient;

char client_ID[40];
char sea_level_pressure[40];


void setup()
{
  Serial.begin(9600);  // Init serial link for debugging


  
  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // Uncomment and run it once, if you want to erase all the stored information
  //  wifiManager.resetSettings();
  
  WiFiManagerParameter id_client("ID", "client_ID", client_ID, 40);
  WiFiManagerParameter sea_level("sea_level_pressure", "sea_level_pressure", sea_level_pressure, 40);
  
  wifiManager.addParameter(&id_client);
  wifiManager.addParameter(&sea_level);
  
  wifiManager.autoConnect("ESP8266 Brand New");

  // if you get here you have connected to the WiFi
  Serial.println("Connected.");


  Serial.print("Esp ip adresss: ");
  Serial.println(WiFi.localIP());
}


void loop()
{
  //sleep for 10 seconds
  //  ESP.deepSleep(10000000, WAKE_RF_DEFAULT);
}
