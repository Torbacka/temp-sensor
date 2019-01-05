#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <config.h>

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#define ONE_WIRE_BUS 14
ESP8266WiFiMulti WiFiMulti;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

char static_ip[16] = "10.0.1.59";
char static_gw[16] = "10.0.1.1";
char static_sn[16] = "255.255.255.0";

void setup(void) {
   
    Serial.begin(115200);
    sensors.begin();

    for (uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    WiFiManager wifiManager;
    
    //tries to connect to last known settings
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP" with password "password"
    //and goes into a blocking loop awaiting configuration
    if (!wifiManager.autoConnect("AutoConnectAP")) {
        Serial.println("failed to connect, we should reset as see if it connects");
        delay(3000);
        ESP.reset();
        delay(5000);
     }

    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    Serial.println("local ip");
    Serial.println(WiFi.localIP());
    
}

void loop(void) { 
    // Wait for WiFi connection
    if ((WiFiMulti.run() == WL_CONNECTED)) {
        sendPost("http://temp.torbacka.se", getTemperatur());
    } else {
        Serial.printf("[HTTP] Unable to connect\n");
    }

    sleep(1000);
}


// TODO: Deep sleep to save battery
void sleep(long time) {
  delay(time);
}

// TODO: Should move it out to my own HTTP library for future use
void sendPost(String url, String data) {
    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // Start of request
    if (http.begin(client, url)) {
        Serial.print("[HTTP] POST...\n");
        http.addHeader("Content-Type", "application/json;charset=UTF-8");
        int httpCode = http.POST(data);
  
        // httpCode will be negative on error
        if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    
            // file found at server
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
              String payload = http.getString();
              Serial.println(payload);
            }
        }else {
            Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
    } 
    http.end();
}

String getTemperatur() {
    sensors.requestTemperatures(); 
    String temperatur = String(sensors.getTempCByIndex(0), 3);
    return String("{ \"temparatur\": " + temperatur + " }"); 
}
