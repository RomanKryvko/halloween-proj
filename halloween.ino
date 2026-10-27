#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SPI.h>
#include "skeleton.h"

ESP8266WebServer server(80);
Skeleton skeleton;

const char SSID[] = "skelenet";
const char PWD[] = "1234567890";
const char DOMAIN[] = "skeleton";

int lastState = -1;
int stateCounter = 0;

int baseDelay = 1500;
int randDelay = 1500;
int upDuration = 2000;
int downDuration = 2000;

void handleRoot() {
  String page = R"HTML(<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Skeleton settings</title>
  <style>
    body { font-family: sans-serif; max-width: 28rem; margin: 2rem auto; padding: 0 1rem; }
    label { display: block; margin: 1rem 0; }
    input { display: block; box-sizing: border-box; width: 100%; padding: .5rem; }
    button { padding: .5rem 1rem; }
    #response { white-space: pre-wrap; }
  </style>
</head>
<body>
  <h1>Skeleton settings</h1>
  <p>All times are in milliseconds.</p>
  <form id="settings" action="/update" method="post">
    <label>Base delay <i>(min: 0, max: 3600000)</i>
      <input type="number" name="baseDelay" value="%BASE_DELAY%" min="0" max="3600000" step="1" required>
    </label>
    <label>Random delay <i>(min: 0, max: 3600000)</i>
      <input type="number" name="randDelay" value="%RAND_DELAY%" min="1" max="3600000" step="1" required>
    </label>
    <label>Up duration <i>(min: 1000, max: 5000)</i>
      <input type="number" name="upDuration" value="%UP_DURATION%" min="1000" max="5000" step="1" required>
    </label>
    <label>Down duration <i>(min: 1000, max: 5000)</i>
      <input type="number" name="downDuration" value="%DOWN_DURATION%" min="1000" max="5000" step="1" required>
    </label>
    <button id="formbutton" type="submit">Apply</button>
  </form>
  <button id="raisebtn" onclick="raise">Raise</button>
  <p id="response" role="status" aria-live="polite"></p>
  <script>
    const form = document.getElementById('settings');
    const output = document.getElementById('response');
    form.addEventListener('submit', async (event) => {
      event.preventDefault();
      const button = document.getElementById('formbutton');
      button.disabled = true;
      output.textContent = 'Applying...';
      try {
        const response = await fetch(form.action, {
          method: 'POST',
          body: new URLSearchParams(new FormData(form))
        });
        const message = await response.text();
        output.textContent = response.ok ? message : 'Update failed: ' + message;
      } catch (error) {
        output.textContent = 'Could not apply settings: ' + error.message;
      } finally {
        button.disabled = false;
      }
    });
    const raise = () => {
      const button = document.getElementById('raisebtn');
      button.disabled = true;
      try {
        const response = await fetch("/raise", { method: 'POST' });
        output.textContent = response.ok ? message : 'Operation failed: ' + message;
      } catch (error) {
        output.textContent = 'Could not apply settings: ' + error.message;
      } finally  {
        button.disabled = false;
      }
    };
  </script>
</body>
</html>)HTML";
  page.replace("%BASE_DELAY%", String(baseDelay));
  page.replace("%RAND_DELAY%", String(randDelay));
  page.replace("%UP_DURATION%", String(upDuration));
  page.replace("%DOWN_DURATION%", String(downDuration));
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/html; charset=utf-8", page);
}

int getIntArg(const String& name, int min, int max) {
  if (server.hasArg(name)) {
    return constrain(server.arg(name).toInt(), min, max);
  }
  return -1;
}

void handleIntUpdates() {
  String updated ("Updated the following values:  ");
  int newValue = getIntArg("baseDelay", 0, 3'600'000);
  if (newValue != -1 && newValue != baseDelay) {
    baseDelay = newValue;
    updated += "base delay, ";
  }

  newValue = getIntArg("randDelay", 1, 3'600'000);
  if (newValue != -1 && newValue != randDelay) {
    randDelay = newValue;
    updated += "random delay, ";
  }

  newValue = getIntArg("upDuration", 1000, 5000);
  if (newValue != -1 && newValue != upDuration) {
    upDuration = newValue;
    updated += "up duration, ";
  }

  newValue = getIntArg("downDuration", 1000, 5000);
  if (newValue != -1 && newValue != downDuration) {
    downDuration = newValue;
    updated += "down duration, ";
  }
  updated.remove(updated.length() - 2, 2);
  server.send(200, "text/plain", updated);
}

void handleRaise() {
  skeleton.restart();
  server.send(200, "text/plain", "Restarted skeleton loop.");
}

void setup() {
  skeleton = Skeleton(D4, D2);
  Serial.begin(9600);
  while (!Serial) {
    delay(500);
  }

  bool result = WiFi.softAP(SSID, PWD);
  if(result == true) {
    Serial.println("AP ready");
  } else {
    Serial.println("AP failed");
    while (1);
  }
  
  if (MDNS.begin(DOMAIN, WiFi.softAPIP())) {
    MDNS.addService("http", "tcp", 80);
    Serial.print("Set up MDNS: http://");
    Serial.print(DOMAIN);
    Serial.println(".local");
  } else {
    Serial.println("Error setting up MDNS responder!");
    while (1);
  }

  printWifiStatus();

  server.on("/", HTTP_GET, handleRoot);
  server.on("/update", HTTP_POST, handleIntUpdates);
  server.on("/raise", HTTP_POST, handleRaise);
  server.begin();
}

void loop() {
  MDNS.update();
  server.handleClient();
  skeleton.move(upDuration, downDuration, baseDelay, randDelay);
  int cur_state = (int)skeleton.get_state();
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

  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  Serial.print("Signal strength (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}
