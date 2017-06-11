This is a fork of the BurgessWorld [example sketch](https://github.com/cburgess5294/BurgessWorld) implementing MQTT instead of ThingSpeak.com to report DHT temp/humidity.

## BurgessWorld Battery Powered Temperature Board

- [BurgessWorld Battery Temperature board](https://www.tindie.com/products/cburgess129/battery-powered-wifi-temphumidity-sensor/).  [Github repository](https://github.com/cburgess5294/BurgessWorld)

## Arduino Libraries Used

- WiFiEsp.h, [bportaluri](https://github.com/bportaluri/WiFiEsp) library [know](https://github.com/bportaluri/WiFiEsp/issues/37) to work with PubSubClient
- PubSubClient.h, [Arduino Client for MQTT](http://pubsubclient.knolleary.net/)

## Change Log

### v0.70

- Battery level reporting via MQTT

### v0.6

- Replace WiFi library with [bportaluri WiFiEsp.h](https://github.com/bportaluri/WiFiEsp)
- Use MQTT instead of ThingSpeak to report DHT temp/humidity
- Report Arduino IP address on boot via MQTT

## MQTT & Arduino

This projects is inspired by the MickMake [Letterbox project](https://mickmake.com/archives/2648) with the github repository [here](https://github.com/MickMakes/MQTTletterbox) and the BurgessWorld [Battery-Powered WiFi Temp/Humditity Sensor board](https://www.tindie.com/products/cburgess129/battery-powered-wifi-temphumidity-sensor/).

MQTT example code was taken from the [bportaluri/WiFiEsp](https://github.com/bportaluri/WiFiEsp/issues/37) project.

To use the [Arduino ESP8266 library](https://github.com/esp8266/Arduino) in the Arduino Preferences add the following to the list of Additional Boards Manager URLs:

- Enter http://arduino.esp8266.com/stable/package_esp8266com_index.json into Additional Board Manager URLs field. You can add multiple URLs, separating them with commas.

- Open Boards Manager from Tools > Board menu and install esp8266 platform (and don't forget to select your ESP8266 board from Tools > Board menu after installation).

Added [Arduino Client for MQTT](http://pubsubclient.knolleary.net/).

## ESP-01 Firmware information

```
AT version:1.2.0.0(Jul  1 2016 20:04:45)
SDK version:1.5.4.1(39cb9a32)
Ai-Thinker Technology Co. Ltd.
Dec  2 2016 14:21:16
```

# Misc Notes Taken During The Poroject

## ESP8266 Boards

Review of [ESP8266 boards](https://openhomeautomation.net/choose-esp8266-module/)

## ESP-01 Firmware Options

- [Espressif](http://bbs.espressif.com/viewforum.php?f=46)
- [NodeMCU](https://nodemcu.readthedocs.io/en/master/en/flash/)

ESP-01 [serial port connection](http://fab.cba.mit.edu/classes/863.14/tutorials/Programming/serialwifi.html)

Debut ESP-01 from [Arduino code](http://yaab-arduino.blogspot.com/2015/03/debugging-esp8266-commands-with-arduino.html)

Good ESP-01 [introduction and tutorial](http://www.instructables.com/id/Getting-Started-With-the-ESP8266-ESP-01/)

Description of [ESP-01 tools](https://gist.github.com/stonehippo/3d9eb100d4f545015515)

## ESP8266 Libraries

- Standard WiFi Shield [Library](https://www.arduino.cc/en/Reference/WiFi)
- ESP8266.h, [Itead](https://github.com/itead/ITEADLIB_Arduino_WeeESP8266) used by Burgessworld
- ESP8266WiFi.h, [esp8266](https://github.com/esp8266/Arduino) used by MickMake Letterbox
- Adafruit_ESP8266.h, Adafruit ESP8266
- WiFiEsp.h, [bportaluri](https://github.com/bportaluri/WiFiEsp) library [know](https://github.com/bportaluri/WiFiEsp/issues/37) to work with PubSubClient

## MQTT Libraries

- PubSubClient.h, [Arduino Client for MQTT](http://pubsubclient.knolleary.net/) used by MickMake Letterbox
- Adafruit_MQTT.h, [Arduino MQTT Library](https://github.com/adafruit/Adafruit_MQTT_Library)
- MQTTClient.h, [256dpi Library](https://github.com/256dpi/arduino-mqtt) Paho based(limited ESP8266 boards)
