#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 14

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup(void) {
    Serial.begin(115200);
    sensors.begin();

    for (uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP("SSID", "PASSWORD");
}

void loop(void) { 
    // wait for WiFi connection
    if ((WiFiMulti.run() == WL_CONNECTED)) {
  
        Serial.print(" C  ");
        Serial.print(Celcius);
        Serial.print(" F  ");s
        Serial.println(Fahrenheit);
        delay(1000);
    }
}

float getTemperatur() {
    sensors.requestTemperatures(); 
    return sensors.getTempCByIndex(0); 
}
