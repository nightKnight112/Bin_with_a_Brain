// Complete code with Bugs
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Servo.h>


#define WIFI_SSID_1 "Pritam_2.4GHz"
#define WIFI_PWD_1 ""
#define WIFI_SSID_2 "A1_Plus"
#define WIFI_PWD_2 ""
#define WIFI_SSID_3 "TP-Link_Extender"
#define WIFI_PWD_3 ""

//Testing and Ultrasonic pins
#define switch1 LED_BUILTIN
#define switch2 6
#define switch3 7
#define ultra 15
//Servo Pins
static const int bin1_pin = 10;
static const int bin2_pin = 12;
// Servo instance
Servo Bin_1;
Servo Bin_2;

WiFiClient myNetwork;

int temp = 3;

// WiFiMulti wifiMulti;
// WiFi connect timeout per AP. Increase when connecting takes longer.
// const uint32_t connectTimeoutMs = 10000;

//MQTT Configuration
#define AIO_SERVER      "192.168.23.105" //IP address of Local Server
#define AIO_SERVERPORT  1883 // use 8883 for SSL
#define AIO_USERNAME    ""
#define AIO_KEY         ""
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&myNetwork, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish Ultrasonic_output = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ultra_out");
Adafruit_MQTT_Publish Nothing = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/test");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe ML_Verdict = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ml_result");
Adafruit_MQTT_Subscribe Test1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/switch1");
Adafruit_MQTT_Subscribe Test2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/switch2");

//MQTT Connection
void MQTT_connect();
// Servo controller
void Bin1_Open();
void Bin2_Open();
void Bin1_Close();
void Bin2_Close();
//LED Indicator
void success(int var);

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(LED_BUILTIN,OUTPUT);
  //**************************************************************************//
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID_1);

  WiFi.begin(WIFI_SSID_1,WIFI_PWD_1);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  int var = 3;
  success(var);
  digitalWrite(LED_BUILTIN,HIGH);
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  //****************************************************************************//
  
  pinMode(switch1, OUTPUT);
  pinMode(switch2, OUTPUT);
  pinMode(switch3, OUTPUT);
  pinMode(ultra,INPUT);
  /*
  // Setup MQTT subscription
  mqtt.subscribe(&ML_Verdict);
  mqtt.subscribe(&Test1);
  mqtt.subscribe(&Test2);
  */
  // Servo Part
  Bin_1.attach(bin1_pin);
  Bin_2.attach(bin2_pin);
  /*
  // Wifi Part
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP("Pritam_2.4GHz","Micronet-0-");
  wifiMulti.addAP("A1_Plus","pritampritam");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
      Serial.println("no networks found");
  } 
  else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      delay(10);
    }
  }

  // Connect to Wi-Fi using wifiMulti (connects to the SSID with strongest connection)
  Serial.println("Connecting Wifi...");
  if(wifiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } */
  
}

void loop() {
 Serial.println("LoopEntrance...");

  //if the connection to the stongest hotstop is lost, it will connect to the next network on the list
  /*if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
    Serial.print("WiFi connected: ");
    Serial.print(WiFi.SSID());
    Serial.print(" ");
    Serial.println(WiFi.RSSI());
  }
  else {
    Serial.println("WiFi not connected!");
  }
  delay(1000); */

  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.

  /*
  MQTT_connect();
  // Publish Ultrasonic Value
  //Getting Ultrasonic Value
  float value_found = -1000.0;
  //Finish UltraSonic Function.............

  // Publish
  if (value_found>0) {
    Serial.println("Object Detected! Might be a Human!! Publishing for verification...");
    if (Ultrasonic_output.publish(value_found)) {
      Serial.println("Published");
      delay(2000);
    }
    else {
      Serial.println("Failed to Publish Ultrasonic value!");
      delay(2000);
    }
  }
  
  if (Nothing.publish("Testing Nothing...") && temp-->0) {
    Serial.println("Published Nothing :)");
    delay(5000);
  }
  else {
    Serial.println("Failed to Publish Nothing!");
    delay(3000);
  }
  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here
  
  // Adafruit_MQTT_Subscribe *subscription;
  //while ((subscription = mqtt.readSubscription(10000))) {
   /* if (mqtt.readSubscription(10000) == &ML_Verdict) {
      Serial.print("Got ML Verdict for Bin: ");
      //Serial.println((char *)ML_Verdict.lastread);
      //int ML_Verdict_State = atoi((char *)ML_Verdict.lastread);
      // To be completed...
      
      
    } *//*
    if (subscription == &Test1) {
      Serial.print(F("Got: "));
      //Serial.println((char *)Test1.lastread);
      //int Test1_State = atoi((char *)Test1.lastread);
      digitalWrite(switch1, HIGH);
    }
    if (subscription == &Test2) {
      Serial.print(F("Got: "));
      //Serial.println((char *)Test2.lastread);
      int Test2_State = atoi((char *)Test2.lastread);
      //digitalWrite(switch1, Test2_State);
      
    }
    if (subscription == &Test3) {
      Serial.print(F("Got: "));
      Serial.println((char *)Test3.lastread);
      //int Test3_State = atoi((char *)Test3.lastread);
      //digitalWrite(switch1, Test3_State);
    } 
  }   */


}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.

void MQTT_connect() {
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
  int var = 8;
  success(var);
  Serial.println("MQTT Connected!");
}

void Bin1_Open() {
  int position = 100;
  Bin_1.write(position);
  delay(20);
}
void Bin2_Open() {
  int position = 100;
  Bin_2.write(position);
  delay(20);
}
void Bin1_Close() {
  int position = 0;
  Bin_1.write(position);
  delay(20);
}
void Bin2_Close() {
  int position = 0;
  Bin_2.write(position);
  delay(20);
}

void success(int var) {
  while(var-->0) {
    digitalWrite(LED_BUILTIN,HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN,LOW);
    delay(200);
  }
}
