#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>
ESP8266WebServer Server;
AutoConnect      Portal(Server);
void rootPage() {
  char content[] = "Hello, world";
  Server.send(200, "text/plain", content);
}
void setup() {
  delay(1000);
  Serial.begin(9600);
  Serial.println();
  Server.on("/", rootPage);
  if (Portal.begin()) {
    Serial.println("HTTP server:" + WiFi.localIP().toString());
  }
}
void loop() {
  Portal.handleClient();
}
