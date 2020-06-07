#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>
#include <PubSubClient.h>

// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;

const char* ssid     = "x";   
const char* password = "x";     
void setup() {
  sensors.begin();
  Serial.begin(57600);

  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  sensors.setResolution(insideThermometer, 11);
  delay(10);
  WiFi.begin(ssid, password);

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
   


  sensors.requestTemperatures(); 
  String temperature = String(sensors.getTempC(insideThermometer), 3);
  sendPost("https://temp.torbacka.se", temperature);
  ESP.deepSleep(36e8);
}

void sendPost(String url, String data) {
    WiFiClient wifiClient;
    PubSubClient client(wifiClient);
    client.setServer("192.168.86.35", 18831);
    // Start of request
    while (!client.connected()) {
      Serial.println("Connecting to MQTT...");
      
      if (client.connect("ESP8266Client", "taco", "x")) {
      
        Serial.println("connected");  
      
      } else {
      
        Serial.print("failed with state ");
        Serial.print(client.state());
        delay(2000);
     
      }
    }
    char buf[40];
    boolean published = client.publish("temp", data.c_str());
    sprintf(buf, "PubSubClient status: %d  published: %d", client.state(), published);
    Serial.println(buf);
    client.disconnect();
    while (client.connected()) {
      delay(50);
    }
    
}

void loop() {
  
}
