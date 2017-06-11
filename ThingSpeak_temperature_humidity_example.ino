/*  This is a fork of the BurgessWorld example sketch implementing MQTT instead of ThingSpeak.com to report
 *  DHT temp/humidity
 *
 *  The LEDs can be disabled by commenting out "#define EnableLEDs" in the sketch below
 *  It is based on the ESP8266 wifi library available here:  https://github.com/itead/ITEADLIB_Arduino_WeeESP8266
 *  The DHT library available here:  https://github.com/adafruit/DHT-sensor-library/archive/master.zip
 *  And the Adadruit sensor library available here:  https://github.com/adafruit/Adafruit_Sensor/archive/master.zip
 *  After uploading the data, the ESP8266 module is put to sleep until it is time to read the sensor again
 *  You should define your local WiFi settings, and your MQTT parameters.
 *  You can add additional sensors and upload additional data by including the appropriate library
 *  and tweeking the "Build Data Package" section in the sendMessage function
 *  This sketch is provided AS-IS with no implied warranty for fitness of purpose
 *  You may modify this sketch to meet your needs
 *
 *  Arduino C++ Reference: https://www.arduino.cc/en/reference/homePage
 */
#include <doxygen.h>
#include "WiFiEsp.h"
#include <PubSubClient.h>
#include "SoftwareSerial.h"
#include "ESP8266.h"
#include <DHT.h>

#define PROG_NAME "BurgessWorldMQTT, "
#define PROG_VERS "v0.6"

//comment out the following line to disable status LEDS when running on batteries
#define EnableLEDs

// Sleep time - 1m = 60000, 5m = 300000, 15m = 900000
#define DelayMS 60000
unsigned long confTempDelay = DelayMS;    // Default temperature publish delay.

//Define status indicator LED pins
#define GREEN 5
#define RED 13

//define wifi SSID and password
#define WIFI_SSID        "nworks24"  // nworks24, VillaLibertate
#define WIFI_PASSWORD    "golfbgc1"  // golfbgc1, clanWa11ac3

//MQTT params
#define MQTT_SERVER  "15.1.4.4"
#define MQTT_PORT    8883
#define MQTT_TOPICBASE "arduino/"

// Define DHT Sensor type - uncomment whatever type you're using
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

//define DHT sensor pin
#define DHTPIN 16

// Define ESP8266 RESET pin
#define ESPRESETPIN 2

//define structure for returning sensor readings
struct reading {
    int h;
    int f;
} reading_r ;

//define Software Serial instance
SoftwareSerial Serial1(7, 6);  // RX, TX

WiFiEspClient espClient;
//WiFiClient client;
PubSubClient mqttClient(espClient);

//create ESP8266 instance
ESP8266 wifi(Serial1, 57600);

//initialize DHT
DHT dht(DHTPIN, DHTTYPE);

//initialize other global variables
bool connectStat;
int failCount;
bool espPresent = false;

void printFullWifiStatus() {
  Serial.print("WiFi Status: ");
  switch (WiFi.status()) {
    case WL_CONNECTED:
      Serial.print("WL_CONNECTED");
      break;
    case WL_NO_SHIELD:
      Serial.print("WL_NO_SHIELD");
      break;
    case WL_IDLE_STATUS:
      Serial.print("WL_IDLE_STATUS");
      break;
/*
    case WL_NO_SSID_AVAIL:
      Serial.println("WL_NO_SSID_AVAIL");
      break;
    case WL_SCAN_COMPLETED:
      Serial.println("WL_SCAN_COMPLETED");
      break;
*/
    case WL_CONNECT_FAILED:
      Serial.print("WL_CONNECT_FAILED");
      break;
/*
    case WL_CONNECTION_LOST:
      Serial.println("WL_CONNECTION_LOST");
      break;
*/
    case WL_DISCONNECTED:
      Serial.print("WL_DISCONNECTED");
      break;
    default:
      Serial.print("**Unknown response: ");
      Serial.print(WiFi.status());
    break;
  }

  // print the SSID of the network you're attached to
  Serial.print(", SSID: ");
  Serial.print(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print(", IP: ");
  Serial.print(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print(", Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ;
}

void setupWifi() {
  Serial.println(F(">> setupWifi <<"));
  delay(10);
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

//  while (WiFi.status() != WL_CONNECTED) {
//    delay(2000);
//    Serial.print(".");
//  }

  printFullWifiStatus();

  Serial.print("Set MQTT server: ");
  Serial.println(MQTT_SERVER);
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
}

void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  payload[length] = 0;    // Hack to be able to use this as a char string.

  if (strstr(topic, MQTT_TOPICBASE "config/"))
  {
    if (strstr(topic, "TempDelay"))
          confTempDelay = atoi((const char *)payload);
  }
}

void mqttReconnect()
{
  // Loop until we're reconnected
  while (!mqttClient.connected())
  {
    printFullWifiStatus();
    // Attempt to connect
    //if (mqttClient.connect("LetterBox", "letterbox", "letterbox", (char *)MQTT_TOPICBASE "State", 1, 0, "DEAD"))
    if (mqttClient.connect((char *)MQTT_TOPICBASE "state"))
    //if (mqttClient.connect("ESP8266Client"))
    {
      Serial.println(F("connected to mqtt server"));
      // Once connected, publish an announcement...
      Publish((char *)"state", (char *)"boot");
      //IPAddress ip = WiFi.localIP();
      String strIp = IpAddress2String(WiFi.localIP());
      Publish((char *)"ip", strIp.c_str());
      // Subscribe to enable bi-directional comms.
      mqttClient.subscribe(MQTT_TOPICBASE "config/#");  // Allow bootup config fetching using MQTT persist flag!
      mqttClient.subscribe(MQTT_TOPICBASE "put/#");     // Send commands to this device, use Home/LetterBox/Get/# for responses.
    }
    else
      delay(5000);
  }
}

void setup() {
  Serial.begin(57600);
  Serial.print(F("Started, "));
  Serial.print(PROG_NAME);
  Serial.println(PROG_VERS);

  //set pin mode for status indicator LED pins
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);

  //set pin mode for DHT22 output pin
  pinMode(DHTPIN, INPUT_PULLUP);

  //SET ESP RESET PIN TO OUTPUT
  pinMode(ESPRESETPIN, OUTPUT);

  //reset ESP8266
  digitalWrite(ESPRESETPIN, LOW);
  delay(20);
  digitalWrite(ESPRESETPIN, HIGH);

  //wait for ESP status
  if (recFind("ready", 5000)){
    Serial.println(F("ESP Ready"));

    WiFi.init(&Serial1);

    printFullWifiStatus();
    // check for the presence of the shield
    if (WiFi.status() == WL_NO_SHIELD) {
      Serial.println("WiFi shield not present");
      // don't continue
      while (true);
    }

    flashLED(GREEN, 1);

    // connect to WiFi network
    setupWifi();
    espPresent = true;
  }
  else{
    Serial.println(F("ESP not found"));
    Serial.println(F("...check ESP UART Speed"));
    Serial.println(F("...verify ESP LED activity"));
    flashLED(RED, 5);
    delay(3000); //delay 15 minutes
    Serial.println(F("...verify ESP LED activity"));
    flashLED(RED, 5);
    delay(3000); //delay 15 minutes
    flashLED(RED, 5);
    delay(3000); //delay 15 minutes
  }

}

void loop() {
  if (espPresent){
    if (!mqttClient.connected()) {
      mqttReconnect();
    }
    //read sensor
    reading_r = readSensor();
    printFullWifiStatus();
    //send message
    PublishInt("temp", reading_r.f);
    PublishInt("humidity", reading_r.h);
    //put ESP to sleep
    Serial.print(F("Put ESP to sleep... "));
    bool espAsleep = false;
    while (!espAsleep){
      Serial1.println(F("AT+SLEEP=2")); //put the ESP8266 to sleep
      if (recFind("OK", 5000)){
        Serial.println(F("ESP sleeping"));
        espAsleep = true;
      }
      else {
        Serial.println(F("ESP failed to sleep"));
        flushSerial();
      }
    }
    delay(DelayMS); //delay 15 minutes
    Serial.print(PROG_NAME);
    Serial.println(PROG_VERS);
    Serial.println(F("Initialize ESP"));
    setupWifi();
  }
}

//function to flush software serial read buffer
void flushSerial(){
    while (Serial1.available() > 0){
      Serial1.read();
    }
}

//function to flash LEDs a specified number of times
uint8_t  flashLED(uint8_t color, uint8_t flashCount){
  #ifdef EnableLEDs
    for (uint8_t flsh = 0; flsh < flashCount; flsh++ ){
      digitalWrite(color, HIGH);
      delay(150);
      digitalWrite(color, LOW);
      delay(100);
    }
  #endif
}

//Bool function to seach Serial RX buffer for string value
bool recFind(String target, uint32_t timeout)
{
  char rdChar;
  String rdBuff = "";
  unsigned long startMillis = millis();
    while (millis() - startMillis < timeout){
      while (Serial1.available() >1){
        rdChar = Serial1.read();
        rdBuff += rdChar;
        if (rdBuff.indexOf(target) != -1) {
          break;
        }
     }
     if (rdBuff.indexOf(target) != -1) {
          break;
     }
   }
   if (rdBuff.indexOf(target) != -1){
    return true;
   }
   return false;
}

struct reading readSensor(){
  uint8_t sumReadHUM = 0;
  uint8_t sumReadTMP = 0;
  uint8_t trashReadHUM = 0;
  uint8_t trashReadTMP = 0;
  //Enable DHT22 VCC pin
  pinMode(DHTPIN, INPUT);
  delay(100);
  Serial.println(F("read sensor"));
  //Serial.println(DateTime.now());
  //read sensor and discard first reading
  trashReadTMP += dht.readTemperature(true);
  trashReadHUM += dht.readHumidity();
  delay(2200);
  //read sensor 2 more times and average
  sumReadTMP += dht.readTemperature(true);
  sumReadHUM += dht.readHumidity();
  delay(2200);
  sumReadTMP += dht.readTemperature(true);
  sumReadHUM += dht.readHumidity();
  pinMode(DHTPIN, INPUT_PULLUP);
  //add readings to struct and return
  reading_r.h = (sumReadHUM / 2);
  reading_r.f = (sumReadTMP / 2);
  Serial.print(F("Humidity: "));
  Serial.println(reading_r.h);
  Serial.print(F("Fahreheit: "));
  Serial.println(reading_r.f);
  return reading_r;
}

void Publish(char *Topic, char *Message)
{
  char TopicBase[80] = MQTT_TOPICBASE;

  strcat(TopicBase, Topic);
  mqttClient.publish(TopicBase, Message);
}


void PublishInt(char *Topic, int Value)
{
  char TopicBase[80] = MQTT_TOPICBASE;
  char Message[10] = "NULL";

  if (!isnan(Value))
    itoa(Value, Message, 10);

  strcat(TopicBase, Topic);
  Serial.print("PublishInt ");
  Serial.print(TopicBase);
  Serial.print(" = ");
  Serial.println(Message);
  mqttClient.publish(TopicBase, Message);
}


void PublishFloat(char *Topic, float Value)
{
  char TopicBase[80] = MQTT_TOPICBASE;
  char Message[10] = "NULL";

  if (!isnan(Value))
    dtostrf(Value, 5, 2, Message);

  strcat(TopicBase, Topic);
  mqttClient.publish(TopicBase, Message);
}

