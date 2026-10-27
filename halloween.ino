#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include "pwd.h"
#include "skeleton.h"

ESP8266WebServer server(80);
Skeleton skeleton;

void handleRoot() {
  server.send(200, "text/plain", "Hello from ESP8266!");
}

void setup() {
  skeleton = Skeleton(D4, D2);
  Serial.begin(9600);

  WiFi.begin(SSID, PWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  while (!Serial) {
    delay(500);
  }
  printWifiStatus();

  server.on("/", HTTP_GET, handleRoot);
  server.begin();
}

int last = -1;
int counter = 0;
void loop() {
  server.handleClient();
  skeleton.move();
  int cur_state = (int) skeleton.get_state();
  if (last != cur_state) {
    Serial.println((std::to_string(cur_state) + ": " + std::to_string(counter) + " iterations").c_str());
    last = cur_state;
    counter = 0;
  } else {
    counter++;
  }
}

void printWifiStatus() {
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Signal strength (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}
