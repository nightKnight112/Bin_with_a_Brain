// Clean Wifi scanning and connection code
// 03.05.2023

#include<WiFi.h>
#include<Adafruit_MQTT.h>
#include<Adafruit_MQTT_Client.h>
#include<Servo.h>

#define ultra_trig_pin 5 //D18 == 18 | output
#define ultra_echo_pin 18 // input
#define servo_1_pin 19
#define servo_2_pin 21
#define testing_pin 23
#define testing_pin2 4

#define WIFI_SSID_1 "Pritam_2.4GHz"
#define WIFI_PWD_1 "Micronet-0-"
IPAddress ip1(192,168,0,120);
IPAddress gateway1(192,168,0,1);
#define WIFI_SSID_2 "A1_Plus"
#define WIFI_PWD_2 "pritampritam"
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

Adafruit_MQTT_Publish Obj_Distance = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Obj_distance_pub");

Adafruit_MQTT_Subscribe ML_Verdict = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ml_result");

void MQTT_Connect();
void Wifi_Connect();
void Bin1_Open();
void Bin2_Open();
void Bin1_Close();
void Bin2_Close();
float get_ultra_data();

void setup() {
    Serial.begin(115200);
    delay(10);
    pinMode(testing_pin,OUTPUT);
    pinMode(testing_pin2,OUTPUT);
    pinMode(LED_BUILTIN,OUTPUT);

    pinMode(ultra_echo_pin,INPUT);
    pinMode(ultra_trig_pin,OUTPUT);

    // Servo Part
    Bin_1.attach(servo_1_pin);
    Bin_2.attach(servo_2_pin);

    Wifi_Connect();

    Serial.println(mqtt.subscribe(&ML_Verdict));

    MQTT_Connect();
}

void loop(){

    // Getting Ultrasonic Data
    float distance;
    distance = get_ultra_data();
    Serial.print("Got Distance: ");
    Serial.println(distance);
    // Publishing Ultrasonic Data
    if(distance<=30.0){
      if (Obj_Distance.publish(distance)) {
      Serial.print("Published object distance : ");
      Serial.println(distance);
      delay(500);
      }
      else {
        Serial.println("Failed to Publish Distance data!");
        delay(1000);
      }
    }
    Adafruit_MQTT_Subscribe *subscription;
    while(subscription = mqtt.readSubscription(10000)) {
      
      if (subscription == &ML_Verdict) {
          Serial.print("Got ML Verdict for Bin: ");
          Serial.println((char *)ML_Verdict.lastread);
          int ML_Verdict_State = atoi((char *)ML_Verdict.lastread);
          if(ML_Verdict_State == 1) {
            Serial.println("In-organic waste detected. Opening Bin 1...");
            Bin1_Open();
            delay(1000);
            Serial.println("Bin 1 Opened. Dump your waste! Bin will be closed in 20 sec");
            Serial.print("Bin 1 closing in...  ");
            for(int k=20;k>=1;k--){
              Serial.print(k);
              Serial.print(" ");
              delay(900);
            }
            Serial.println();
            Bin1_Close();
            Serial.println("Bin 1 Closed");
          }
          else if(ML_Verdict_State == 2) {
            Serial.println("Organic waste detected. Opening Bin 2...");
            Bin2_Open();
            delay(900);
            Serial.println("Bin 2 Opened. Dump your waste! Bin will be closed in 20 sec");
            Serial.print("Bin 2 closing in...  ");
            for(int k=20;k>=1;k--){
              Serial.print(k);
              Serial.print(" ");
              delay(900);
            }
            Serial.println();
            Bin2_Close();
            Serial.println("Bin 2 Closed");
          }
      }
    }
    if(mqtt.connected()){
      return;
    }
    else{
      MQTT_Connect();
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

float get_ultra_data(){
  //Clearing Trigger
  digitalWrite(ultra_trig_pin,LOW);
  delay(5);
  //Set Trigger
  digitalWrite(ultra_trig_pin,HIGH);
  delay(10);
  digitalWrite(ultra_trig_pin,LOW);

  // Read Data from ECHO
  long duration;
  duration = pulseIn(ultra_echo_pin,HIGH);
  //Calculate Distance;
  float distance;
  distance = (duration*0.034)/2; // Sound Speed is 0.034cm/uS
  return distance;
}
void Bin1_Open() {
  for (int pos = 0; pos <= 90; pos += 1) {
    // in steps of 1 degree
    Bin_1.write(pos);
    delay(15); // waits 15ms to reach the position
  }
}
void Bin2_Open() {
  for (int pos = 0; pos <= 90; pos += 1) {
    // in steps of 1 degree
    Bin_2.write(pos);
    delay(15); // waits 15ms to reach the position
  }
}
void Bin1_Close() {
  for (int pos = 90; pos >= 0; pos -= 1) {
    Bin_1.write(pos);
    delay(15); // waits 15ms to reach the position
  }
}
void Bin2_Close() {
  for (int pos = 90; pos >= 0; pos -= 1) {
    Bin_2.write(pos);
    delay(15); // waits 15ms to reach the position
  }
}