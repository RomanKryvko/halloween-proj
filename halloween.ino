#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include "pwd.h"
#include "skeleton.h"

ESP8266WebServer server(80);
Skeleton skeleton;

int lastState = -1;
int stateCounter = 0;

int baseDelay = 1500;
int randDelay = 1500;
int upDuration = 2000;
int downDuration = 2000;

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

void loop() {
  server.handleClient();
  skeleton.move(upDuration, downDuration, baseDelay, randDelay);
  int cur_state = (int) skeleton.get_state();
  if (lastState != cur_state) {
    Serial.println((std::to_string(cur_state) + ": " + std::to_string(stateCounter) + " iterations").c_str());
    lastState = cur_state;
    stateCounter = 0;
  } else {
    stateCounter++;
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
