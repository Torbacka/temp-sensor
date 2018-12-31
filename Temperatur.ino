#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <config.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>

#define ONE_WIRE_BUS 14

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
const char *ssid = USERNAME;
const char *password = WIFI_PASSWORD;

void setup(void) {
    Serial.begin(115200);
    sensors.begin();

    for (uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion
 
    delay(500);
    Serial.println("Waiting for connection");
 
  }
}

void loop(void) { 
    // Wait for WiFi connection
    if ((WiFiMulti.run() == WL_CONNECTED)) {
        sendPost("http://192.168.1.154:8090", getTemperatur());
    } else {
        Serial.printf("[HTTP] Unable to connect\n");
    }

    sleep(1000);
}


// TODO: Deep sleep to save battery
void sleep(long time) {
  deplay(time);
}

// TODO: Should move it out to my own HTTP library for future use
void sendPost(String url, String data) {
    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // Start of request
    if (http.begin(client, url)) {
        Serial.print("[HTTP] GET...\n");
        http.addHeader("Content-Type", "application/json");
        int httpCode = http.POST(data);
  
        // httpCode will be negative on error
        if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    
            // file found at server
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
              String payload = http.getString();
              Serial.println(payload);
            }
        }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

String getTemperatur() {
    sensors.requestTemperatures(); 
    String temperatur = String(sensors.getTempCByIndex(0), 3);
    return String("{ \"temparatur\": " + temperatur + " }"); 
}
