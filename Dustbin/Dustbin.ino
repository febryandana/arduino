/*
 * Authors : Moch. Febryandana Nurfahri
 * 
 * This is for automate opening dustbin lid using Ultrasonic to sense things in front of it
 * Using Arduino Uno, Ultrasonic sensor HCSR04, and servo motor
 */

// Make life easier
#include <HCSR04.h>
#include <Servo.h>

HCSR04 hc(10,5);
int servoPin = 2;
Servo Servo1;

void setup() {
  Servo1.attach(servoPin);
  Serial.begin(9600);
}

void loop() {
  unsigned int distance;
  distance = hc.dist();
  if(distance == 0){
    Serial.println("Out of Range");
    Servo1.write(180);
  }
  else{
    Serial.print(distance);
    Serial.print(" cm");
    if(distance <= 15){
      Servo1.write(90);
    }
    else{
      Servo1.write(180);
    }
  }
delay(1000);
}
