#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>


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


void handleFileList() {
  String ssid_name;
  String psswd;
  Serial.println("Cerere de auth POST");


  Serial.print("Numarul de argumente este:");
  Serial.println(server.args());
}



void setup(void) {
  Serial.begin(9600);
  Serial.println("");
  WiFi.mode(WIFI_AP);           //Only Access point
  WiFi.softAP(ssid, password);  //Start HOTspot removing password will disable security

  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print("HotSpt IP:");
  Serial.println(myIP);

  server.on("/", handleRoot);      //Which routine to handle at root location
  server.on("/auth", authRoutine);
  server.on("/test", HTTP_POST, handleFileList);

  server.begin();                  //Start server
  Serial.println("HTTP server started");
}


void loop(void) {
  server.handleClient();          //Handle client requests
}


