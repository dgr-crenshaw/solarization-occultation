#include <ESP8266WiFi.h>

#include <DHT.h>
#include <DHT_U.h>

#include <PubSubClient.h>

#include <wifiConfig.h>
//#include <wifiConfigRaspberryPi.h>

/*
Make this for the weed eradication solarization/occultation experiment.
One sensor reads temperature and humidity from clear poly cover
Other sensor reads temperature and humidity from black poly cover
*/

/************************* WiFi Access Point *********************************
  converted these to libraries so code calculateDividedVoltagecan be uploaded to github
  without privacy compromise

#define WLAN_SSID       "get from wifiConfig.h"
#define WLAN_PASS       "get from wifiConfig.h"
******************************************************************************/

#include <mqttConfig.h>

/******************************** mqtt ***************************************
  converted these to libraries so code can be uploaded to github
  without privacy compromise
#define mqtt_server "get from mqttConfig.h"
#define mqtt_port 1883 // use 8883 for SSL
#define mqtt_user "get from mqttConfig.h"
#define mqtt_password "get from wifiConfigRasperryPi.h"
******************************************************************************/

//int analog input
int readDividedVoltage = A0;
//int value to calculate
int calculateDividedVoltage = 0;
//float value to display
float showCalculatedVoltage = 0;
//create the class
class readBatteryatA0{

  //setup and loop methods
  public:
    void setup() {
       //init serial for debugging
      //Serial.begin(115200);
    }
    void loop(){
    //read A0
    //average 10 readings
    calculateDividedVoltage = 0; //initialize
    for (int i = 0; i <= 9; i++) { //take 10 readings at 10ms intervals
       calculateDividedVoltage = calculateDividedVoltage + analogRead(A0);
      delay(10);
    }

    calculateDividedVoltage = calculateDividedVoltage/10; //take the average
    //map to measured voltages using regression 0.0138*calculateDividedVoltage + 0.7558
    showCalculatedVoltage = calculateDividedVoltage*0.014 + 0.7;
   //Serial.print("Test reading at A: ");

    //calculateDividedVoltage = analogRead(readDividedVoltage);
   //Serial.println(showCalculatedVoltage);
    //delay(9000); //wait 10 seconds -- includes 1 second of reading battery voltage 100x delay not needed for 1 hour loop
    }
};

readBatteryatA0 batteryTest; //instantiate readBatteryatA0

/*************************dht setup**************************
 * DHT22 sensor setup for WEMOS D1 Mini
 * single channel for now
 * WEMOS pin mappings
 * WeMos D1 mini Pin Number   Arduino IDE Pin Number
 * D0                         16
 * D1                          5
 * D2                          4
 * D3                          0
 * D4                         14
 * D6                         12
 * D7                         13
 * D8                         15
 * TX                          1
 * RX                          3
 * https://chewett.co.uk/blog/1066/pin-numbering-for-wemos-d1-mini-esp8266/
 ***********************************************************/

/**********sensors WEMOS**********/
DHT dht_01(14, DHT22); //channel 01 WEMOS board see pin mapping above
DHT dht_02(12, DHT22); //channel 02 WEMOS board see pin mapping above
DHT dht_03(13, DHT22); //channel 03 WEMOS board see pin mapping above
/**********sensors WEMOS**********/

/**********sensors HUZZAH**********/
DHT dht_01(4, DHT22); //channel 01 HUZZAH board
DHT dht_02(2, DHT22); //channel 02 HUZZAH board
//DHT dht_03(7, DHT22); //channel  HUZZAH board
/**********sensors HUZZAH**********/

/************************************************
define feeds
************************************************/
#define publishTopicSensorNumber "feedSensor0x/SensorNumber"
#define publishTopicBatteryFeed "feedSensor0x/batteryRead"
#define publishTopicTemperatureDHT1 "feedSensor0x/temperatureDHT1"
#define publishTopicHumidityDHT1 "feedSensor0x/humidityDHT1"
#define publishTopictemperatureDHT2 "feedSensor0x/temperatureDHT2"
#define publishTopicHumidityDHT2 "feedSensor0x/humidityDHT2"
#define publishTopicTemperatureDHT3 "feedSensor0x/temperatureDHT3"
#define publishTopicHumidityDHT3 "feedSensor0x/humidityDHT3"

WiFiClient wifiClient;
PubSubClient mqttClient;

void setup() {
  dht_01.begin(); //initialize DHT22 01
  dht_02.begin(); //initialize DHT22 02
  //dht_03.begin(); //initialize DHT22 03
 //Serial.begin(115200);
  setup_wifi();
  mqttClient.setClient(wifiClient);
  mqttClient.setServer(mqtt_server, mqtt_port);
  //Serial.print("mqtt Server: ");
  //Serial.println(mqtt_server);
  //Serial.print("mqtt Port: ");
  //Serial.println(mqtt_port);
  batteryTest.loop();
//publish once in setup

  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
  

  static char voltageAsString[15]; //string array for payload
  static char temperatureDHT01[15]; //string array for payload
  static char humidityDHT01[15];//string array for payload
  static char temperatureDHT02[15]; //string array for payload
  static char humidityDHT02[15];//string array for payload
  static char temperatureDHT03[15]; //string array for payload
  static char humidityDHT03[15];//string array for payload

  dtostrf(showCalculatedVoltage, 7, 3, voltageAsString);
  dtostrf(dht_01.readTemperature(true), 7, 3, temperatureDHT01);
  dtostrf(dht_01.readHumidity(), 7, 3, humidityDHT01);
  dtostrf(dht_02.readTemperature(true), 7, 3, temperatureDHT02);
  dtostrf(dht_02.readHumidity(), 7, 3, humidityDHT02);
//  dtostrf(dht_03.readTemperature(true), 7, 3, temperatureDHT03);
//  dtostrf(dht_03.readHumidity(), 7, 3, humidityDHT03);

  mqttClient.publish(publishTopicSensorNumber, "Sensor0x", true);
  mqttClient.publish(publishTopicBatteryFeed, voltageAsString, true);
  mqttClient.publish(publishTopicTemperatureDHT1, temperatureDHT01, true);
  mqttClient.publish(publishTopicHumidityDHT1, humidityDHT01, true);
  mqttClient.publish(publishTopictemperatureDHT2, temperatureDHT02, true);
  mqttClient.publish(publishTopicHumidityDHT2, humidityDHT02, true);
  mqttClient.publish(publishTopicTemperatureDHT3, temperatureDHT03, true);
  mqttClient.publish(publishTopicHumidityDHT3, humidityDHT03, true);

  delay(10000); //delay to allow publish to complete

 //Serial.println("******************************");
 //Serial.println();
 //Serial.println("**********publishing**********");
 //Serial.print("VOLTAGE SENSOR_01: ");
 //Serial.println(showCalculatedVoltage);
 //Serial.println();
 //Serial.print("TEMPERATURE 01 SENSOR_01: ");
 //Serial.println(dht_01.readTemperature(true));
 //Serial.print("HUMIDITY 01 SENSOR_01: ");
 //Serial.println(dht_01.readHumidity());
 //Serial.println();
 //Serial.print("TEMPERATURE 02 SENSOR_01: ");
 //Serial.println(dht_02.readTemperature(true));
 //Serial.print("HUMIDITY 02 SENSOR_01: ");
 //Serial.println(dht_02.readHumidity());
 //Serial.println();
 //Serial.print("TEMPERATURE 03 SENSOR_01: ");
 //Serial.println(dht_03.readTemperature(true));
 //Serial.print("HUMIDITY 03 SENSOR_01: ");
 //Serial.println(dht_03.readHumidity());
 //Serial.println("******************************");
 //Serial.println();
 //Serial.println("*****************************");
 //Serial.println("**********published**********");
 //Serial.println("*****************************");
 //Serial.println();

/**********for deep sleep**********
   *1 hour 36e8 microseconds
   *10 minutes 6e8 microseconds
   *30 seconds 30e6 microseconds
   *10 seconds 10e6 microseconds
   **********************************/
 //Serial.println();
 //Serial.println("**********************");
 //Serial.println("***sleeping 30 sec****");
 //Serial.println("**********************");
 //Serial.println();

  ESP.deepSleep(30e6);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
 //Serial.println();
 //Serial.print("Connecting to ");
 //Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
   //Serial.print(".");
  }

 //Serial.println("");
 //Serial.println("WiFi connected");
 //Serial.println("IP address: ");
 //Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
   //Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect("solarOccult00", mqtt_user, mqtt_password)) {
     //Serial.println("connected");
    } else {
     //Serial.print("failed, rc=");
     //Serial.print(mqttClient.state());
     //Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
 }

void loop() {
/*****no loop deep sleep from setup******/
}
