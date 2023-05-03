// Clean Wifi scanning and connection code
// 03.05.2023

#include<WiFi.h>

#define WIFI_SSID_1 "Pritam_2.4GHz"
#define WIFI_PWD_1 "########"
IPAddress ip1(192,168,0,120);
IPAddress gateway1(192,168,0,1);
#define WIFI_SSID_2 "A1_Plus"
#define WIFI_PWD_2 "########"
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

void Wifi_Connect();

void setup() {
    Serial.begin(115200);
    delay(10);

    pinMode(LED_BUILTIN,OUTPUT);
    //Scan WiFi
    Serial.println("Scanning for available networks...");
    int total_networks = WiFi.scanNetworks();
    if(total_networks==0){
        Serial.println("No Network Found!!!");
    }
    for(int i=0;i<total_networks;i++){
        Serial.print("Network ");
        Serial.print(i+1);
        Serial.print(" : ");
        Serial.print(WiFi.SSID(i));
        Serial.print("\tStrength");
        Serial.print(" : ");
        Serial.println(WiFi.RSSI(i));
    }
    Wifi_Connect();
}

void loop(){

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
