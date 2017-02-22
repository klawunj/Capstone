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
  String InputBuffer;//variable to get the serial value
  int contest;//store the integer value of the serial value
  String Speed = "";//store the integer speed value
  String Turn = "";//store the turn value

  //Flags and checks
  int count = 0;//ensure that each character is read only once
  bool variable = 0;//signifies the switch between speed and turn when reading values
  
  if (Serial.available()){
    InputBuffer = Serial.read();
  }
  
  contest = InputBuffer.toInt();
  contest = contest - 48; //convert ascii values to usable numbers
  
  //Check which value was just read and do the correct action based on that value
  
  //If there is a space, switch which variable written to
  if(contest == -16 && count == 0){
    variable = 1;
    count++;
  }
  
  //if there is a precent sign, new character
  else if (contest == -11){
    count = 0;
  }
  
  //If there is #, end of message
  else if (contest == -13){
    //Turn strings to integers, set output to integers
    output.Speed() = Speed.toInt();
    output.Turn() = Turn.toInt();
      
    //reset values
    contest = 0; Speed = ""; Turn = ""; count = 0; variable = 0;
    //Return the new Drive Commands
    return output;
  }
  
  //if there is a negative sign, put a negative sign
  else if (contest == -3 && count == 0){
    if (variable ==1){Turn = Turn+"-";}
    else{Speed = Speed+"-";}
    count++;
  }

  //If it is none of the characters above, add the number value to the message
  else{
    if(contest >= 0 && count == 0 ){
      if (variable ==1){Turn = Turn+contest;}
      else{Speed = Speed+contest;}
      count++;
    }
  }
}
