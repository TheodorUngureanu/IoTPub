#include <ESP8266WiFi.h>



void setup()
{
  Serial.begin(9600);  // Init serial link for debugging

  // if you get here you have connected to the WiFi
  Serial.println("Hello");
  Serial.println("Device ready");
  Serial.println("going to sleep...");

  ESP.deepSleep(10000000, WAKE_RF_DEFAULT);
  //  Serial.print("Esp ip adresss: ");
  //  Serial.println(WiFi.localIP());
}


void loop()
{
  //  Serial.println("Hello");
  //  Serial.println("going to sleep...");
  //  //  sleep for 10 seconds
  //  ESP.deepSleep(10000000, WAKE_RF_DEFAULT);
}


