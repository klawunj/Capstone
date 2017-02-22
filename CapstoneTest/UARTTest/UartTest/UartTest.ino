#include <SoftwareSerial.h>
#include <stdlib.h>

//Variables
String test;//variable to get the serial value
int contest;//store the integer value of the serial value
String Speed = "";//store the integer speed value
String Turn = "";//store the turn value
int intSpeed, intTurn;

//Flags and checks
int count = 0;//ensure that each character is read only once
bool flag = 0;//ensure that full message is only printed once
bool variable = 0;//signifies the switch between speed and turn when reading values


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
  contest = contest - 48; //convert ascii values to usable numbers
  
  //If there is a space, switch which variable written to
  if(contest == -16 && count == 0){
    variable = 1;
    count++;
  }
  
  //if there is a precent sign, new character
  else if (contest == -11){
    count = 0;
  }
  
  //If there is #, means end of message
  else if (contest == -13){
    //check if the first time sent
    if (flag==0){
      
      intSpeed = Speed.toInt();
      intTurn = Turn.toInt();
      Serial.println("Speed is " + Speed);
      Serial.println(intSpeed);
      Serial.println("Turn is " + Turn);
      Serial.println(intTurn);
      
      //reset values
      contest = 0;
      Speed = "";
      Turn = "";
      flag = 1;
      count = 0;
      variable = 0;
    }
  }
  
  //if there is a negative sign, put a negative sign
  else if (contest == -3 && count == 0){
    if (variable ==1){Turn = Turn+"-";}
    else{Speed = Speed+"-";}
    count++;
    flag = 0;
  }

  //Add the number value to the message
  else{
    if(contest >= 0 && count == 0 ){
      if (variable ==1){Turn = Turn+contest;}
      else{Speed = Speed+contest;}
      count++;
      flag = 0;
    }
  }
}
