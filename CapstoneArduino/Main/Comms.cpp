/*****************************************************************************
 *                                 Includes
 ****************************************************************************/
#include "Comms.h"
#include "Arduino.h"
#include <SoftwareSerial.h>
#include <stdlib.h>
/*****************************************************************************
 *                             Global Variables
 ****************************************************************************/
 

 /*****************************************************************************
 *                                 Functions
 ****************************************************************************/

 void InitComms(){
  Serial.begin(9600);
  while(!Serial){
  }
 }

DriveCommands GetDriveCommands(){
  //output
  DriveCommands output;
  //Variables
  //Variables
  String inputBuffer;//variable to get the serial value
  int conIB;//store the integer value of the serial value
  String Speed = "";//store the integer speed value
  String Turn = "";//store the turn value
  int intSpeed, intTurn;
  
  //Flags and checks
  int count = 0;//ensure that each character is read only once
  bool flag = 0;//ensure that full message is only printed once
  bool variable = 0;//signifies the switch between speed and turn when reading values

  while (1){
    if(Serial.available()){
    inputBuffer = Serial.read();
    }

    conIB = inputBuffer.toInt();
    conIB = conIB - 48; //convert ascii values to usable numbers

    //If there is a space, switch which variable written to
    if(conIB == -16 && count == 0){
      variable = 1;
      count++;
    }

    //if there is a precent sign, new character
    else if (conIB == -11){
      count = 0;
    }

    //If there is #, means end of message
    else if (conIB == -13){
      //check if the first time sent
      if (flag==0){

        intSpeed = Speed.toInt();
        intTurn = Turn.toInt();
        output.Speed = intSpeed;
        output.Turn = intTurn;

        //reset values
        conIB = 0;
        Speed = "";
        Turn = "";
        flag = 1;
        count = 0;
        variable = 0;

        //return value
        return output;
      }
    }

    //if there is a negative sign, put a negative sign
    else if (conIB == -3 && count == 0){
      if (variable ==1){Turn = Turn+"-";}
      else{Speed = Speed+"-";}
      count++;
      flag = 0;
    }

    //Add the number value to the message
    else{
      if(conIB >= 0 && count == 0 ){
        if (variable ==1){Turn = Turn+conIB;}
        else{Speed = Speed+conIB;}
        count++;
        flag = 0;
    }
  }
 }
}
