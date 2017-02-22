
#include <Servo.h>

Servo S1;

void setup() {
 S1.attach(9);
 
 S1.write(30);
 
 delay(3000);

}

void loop() {
  S1.write(20);

  delay(3000);

  S1.write(30);

  delay(3000);

  S1.write(40);

  delay(3000);

  S1.write(30);

  delay(3000);

}
