#include <SoftwareSerial.h>
#include <stdlib.h>

String test;
int contest;
String Done = "";


void setup() {
  Serial.begin(9600);
  
  while(!Serial){
    
  }

  Serial.println("On");
  

}

void loop() {
  if(Serial.available()){
    test = Serial.read();
  }

  contest = test.toInt();

  contest = contest - 48;


  if(contest == -16){
    Done = Done + " ";
  }
  else if (contest == -13){
    Serial.println(Done);
  }
  else if (contest == -3){
    Done = Done + "-";
  }
  else{
    if(contest >= 0){
      Done = Done + contest;
    }
  }


  
//  if (mySerial.available()){
//    Serial.write(mySerial.read());
//  }
//
//  delay(5000);
//
//  if(Serial.available()){
//    mySerial.write(Serial.read());
//  }
//
//  delay(5000);

}
