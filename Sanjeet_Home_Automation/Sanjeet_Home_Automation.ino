#include "JSON.h"
#include "server.h"

#define ADDRESS_1         192
#define ADDRESS_2         168
#define ADDRESS_3         43
#define NODE_ADDRESS      100

IPAddress local_IP(ADDRESS_1, ADDRESS_2, ADDRESS_3, NODE_ADDRESS);
IPAddress gateway(ADDRESS_1, ADDRESS_2, ADDRESS_3, 1);
IPAddress subnet(255, 255, 255, 0);

bool connected = false;
uint32_t wifiConnectionTime = millis();
#define WIFI_SSID         "mikytrap"
#define PASSWORD          "12345678"
#define WIFI_TIMEOUT  30000


void setup() {
  Serial.begin(115200);
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Cannot assign static IP");
  } else {
  }
  WiFi.begin(WIFI_SSID, PASSWORD);
  initializeServer();
  
}

void loop() {
  runWiFiRoutine();
  if (connected) {
    server.handleClient();
  }
  if (Serial.available()) {
    String data = Serial.readString();
    sscanf(data.c_str(), "%f %f", &temprature, &humidity);
    Serial.flush();
  }
  for(int i=0; i<Device::length(); i++) {
    device[i].update();
  }
}


void runWiFiRoutine() {
   if (!connected && WiFi.status() == WL_CONNECTED) {
    connected = true;
    Serial.println("WiFi connected successfully");
    Serial.println(WiFi.localIP());    
  }
  if (connected && WiFi.status() != WL_CONNECTED) {
    connected = false;
    Serial.println("WiFi disconnected");
    WiFi.begin(WIFI_SSID, PASSWORD);
  }
  if (!connected) {
    if (millis() - wifiConnectionTime > WIFI_TIMEOUT) {
      wifiConnectionTime = millis();
      Serial.println("Unable to connect to wifi");
      WiFi.begin(WIFI_SSID, PASSWORD);
    }
  }
}
