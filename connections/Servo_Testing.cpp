// Driver Code for successfully running MG995 with Arduino UNO and ESP32
#include<ESP32Servo.h>
#include<Arduino.h>

#define servoPin 21
Servo myServo;
void setup(){
    Serial.begin(115200);
    myServo.attach(servoPin);
}
void loop(){
    int degree = 110;
    for(int i=0;i<=degree;i++){
        myServo.write(i);
        delay(10);
    }
    delay(1000);
    for(int i=degree;i>=0;i--){
        myServo.write(i);
        delay(10);
    }
    delay(1000);
}