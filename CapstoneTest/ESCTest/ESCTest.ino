#include <Servo.h>

Servo esc;

void setup() {
  Serial.begin(9600);
  
  esc.attach(9);

//  pinMode(9, OUTPUT);

  delay(3000);
}

void loop() {
  esc.write(89);

 // delay(2000);

//  esc.write(0);
//
//  delay(4000);

//  analogWrite(9, 150);
//  
//  delay(2000);

//  analogWrite(9,0);
//
//  delay(3000);

}
