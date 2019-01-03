# Temperature

Reading values from one wire temperature sensor (ds18b20) every 10 min and sending it to my home server. The communication has authentlication but its not encrypted, because its on my home network and its only temperature data.

### Schematics
Brown wire should be 10k resistors.
 
![Schematics](https://github.com/Torbacka/temp-sensor/blob/master/diagram.png)

Running configuration:

![Schematics](https://github.com/Torbacka/temp-sensor/blob/master/diagram2.png)


### Requirements

1. [OneWire.h](https://github.com/PaulStoffregen/OneWire)
2. [DallasTemperature.h](https://github.com/milesburton/Arduino-Temperature-Control-Library)
