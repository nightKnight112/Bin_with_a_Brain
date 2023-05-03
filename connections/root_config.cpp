// WiFi and MQTT connection and Testing of Publish and Subscribe
// 03.05.2023

#include<WiFi.h>
#include<Adafruit_MQTT.h>
#include<Adafruit_MQTT_Client.h>
#include<Servo.h>

#define ultra_input_pin 18 //D18 == 18 
#define servo_1_pin 19
#define servo_2_pin 21
#define testing_pin 23
#define testing_pin2 4

#define WIFI_SSID_1 "Pritam_2.4GHz"
#define WIFI_PWD_1 ""
IPAddress ip1(192,168,0,120);
IPAddress gateway1(192,168,0,1);
#define WIFI_SSID_2 "A1_Plus"
#define WIFI_PWD_2 ""
IPAddress ip2(192,168,43,120);
IPAddress gateway2(192,168,43,1);
#define WIFI_SSID_3 "TP-Link_Extender"
#define WIFI_PWD_3 ""
IPAddress ip3(192,168,23,120);
IPAddress gateway3(192,168,23,1);
IPAddress subnet(255,255,255,0);
IPAddress dns1(8,8,8,8);
IPAddress dns2(4,2,2,2);

WiFiClient myNetwork;

Servo Bin_1;
Servo Bin_2;

#define AIO_SERVER      "192.168.23.105" //IP address of Local Server
#define AIO_SERVERPORT  1883 // use 8883 for SSL
#define AIO_USERNAME    ""
#define AIO_KEY         ""

Adafruit_MQTT_Client mqtt(&myNetwork, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish Test_Pub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/test_pub");

Adafruit_MQTT_Subscribe ML_Verdict = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ml_result");

void MQTT_Connect();
void Wifi_Connect();

void setup() {
    Serial.begin(115200);
    delay(10);
    pinMode(testing_pin,OUTPUT);
    pinMode(testing_pin2,OUTPUT);
    pinMode(LED_BUILTIN,OUTPUT);
    
    Wifi_Connect();

    Serial.println(mqtt.subscribe(&ML_Verdict));

    MQTT_Connect();
}

void loop(){
    delay(1000);
    int temp = 3;
    if (Test_Pub.publish("Testing publish...") && temp-->0) {
    Serial.println("Published Test data :) ");
    delay(500);
    }
    else {
      Serial.println("Failed to Publish Test data!");
      delay(1000);
    }
    Adafruit_MQTT_Subscribe *subscription;
    while(subscription = mqtt.readSubscription(10000)) {
      
      if (subscription == &ML_Verdict) {
          Serial.print("Got ML Verdict for Bin: ");
          Serial.println((char *)ML_Verdict.lastread);
          int ML_Verdict_State = atoi((char *)ML_Verdict.lastread);
          digitalWrite(testing_pin2,ML_Verdict_State);
      }
    }
}

void MQTT_Connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 5;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  digitalWrite(testing_pin,HIGH);
  Serial.println("MQTT Connected!");
  Serial.println(mqtt.ping());
}

void Wifi_Connect(){
    for(int j = 0; j<3;j++) {
        if(j==0) {
            Serial.println(); Serial.println();
            Serial.print("Connecting to ");
            Serial.println(WIFI_SSID_1);
            WiFi.config(ip1,gateway1,subnet,dns1,dns2);
            delay(10);
            WiFi.begin(WIFI_SSID_1,WIFI_PWD_1);
        }
        else if(j==1) {
            Serial.println(); Serial.println();
            Serial.print("Connecting to ");
            Serial.println(WIFI_SSID_2);
            WiFi.config(ip2,gateway2,subnet,dns1,dns2);
            delay(10);
            WiFi.begin(WIFI_SSID_2,WIFI_PWD_2);
        }
        else if(j==2) {
            Serial.println(); Serial.println();
            Serial.print("Connecting to ");
            Serial.println(WIFI_SSID_3);
            WiFi.config(ip3,gateway3,subnet,dns1,dns2);
            delay(10);
            WiFi.begin(WIFI_SSID_3,WIFI_PWD_3);
        }
        int attempts = 1;
        while (WiFi.status() != WL_CONNECTED and attempts++<=10) {
            delay(500);
            Serial.print(".");
        }
        Serial.println();
        if(attempts<10) {
            Serial.println();
            digitalWrite(LED_BUILTIN,HIGH);
            Serial.print("WiFi connected to ");
            Serial.println(WiFi.SSID());
            Serial.println("IP address: "); Serial.println(WiFi.localIP());
            Serial.println("Gateway IP address: "); Serial.println(WiFi.gatewayIP());
            break;
        }
        else {
            Serial.println("WiFi connection failed! ");
            Serial.println("Retrying...");
            WiFi.disconnect();
            delay(50);
        }
    }
}
