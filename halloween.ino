#include <Servo.h>
#include <random>
#include <time.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include "pwd.h"

Servo servo;
const int start = 2300; // down
const int fin = 1400;   // up
const int baseDelay = 1500;
const int randDelay = 1000;

ESP8266WebServer server(80);
void handleRoot() {
  server.send(200, "text/plain", "Hello from ESP8266!");
}
void setup() {
  servo.attach(D4, 500, 2500);
  pinMode(D2, OUTPUT);
  servo.writeMicroseconds(start);
  srandom(time(0));
  
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

  digitalWrite(D2, HIGH);

  for (int i = start; i > fin; i -= 20) {
    servo.writeMicroseconds(i);
    delay(50);
  }

  for (int i = 0; i < 3; i++) {
    digitalWrite(D2, LOW);
    delay(200);
    digitalWrite(D2, HIGH);
    delay(200);
  }
  digitalWrite(D2, LOW);

  for (int i = fin; i < start; i += 20) {
    servo.writeMicroseconds(i);
    delay(50);
  }

  delay(baseDelay + random() % randDelay);
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
