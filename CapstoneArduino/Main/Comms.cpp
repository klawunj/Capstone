/*****************************************************************************
 *                                 Includes
 ****************************************************************************/
#include "Comms.h"
#include "Arduino.h"
#include <SoftwareSerial.h>
#include <stdlib.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
/*****************************************************************************
 *                             Global Variables
 ****************************************************************************/
 int InMsg[1];
 int OutMsg[1];
 RF24 radioRecieve(9,10);
 RF24 radioTransmit(5,6);
 const uint64_t pipe1 = 0xE8E8F0F0E1LL;
 const uint64_t pipe2 = 0xF8F8E0E0F2LL;
 long CarID = 3;
 FSMVarsInt CurrentState;
 MESSAGE OtherCarMessage;

 /*****************************************************************************
 *                                 Functions
 ****************************************************************************/

 void InitComms(){
  Serial.begin(9600);
  radioRecieve.begin();
  radioTransmit.begin();
  radioRecieve.openReadingPipe(1,pipe2);
  radioRecieve.startListening();
  radioTransmit.openWritingPipe(pipe1);
  CurrentState.State = NotNear;
  InMsg[0] = 0;
  OutMsg[0] = 0;
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

 void IntersectionFSM(){
  switch (CurrentState.State){
    
    case Stopped:
      
    break;

    case Proceed:
      
    break;

    case Clear:
     
    break;

    case NotNear:
      
    break;

    case Blocked:
      
    break;
    
    default:
    break;
  }
 }


  
 

  int receive(){  //Recieves message and returns value
  if (radioRecieve.available()){
    while (radioRecieve.available()){
      radioRecieve.read(InMsg, 1);     
    }
  }

  else{
    
    InMsg[0] = 0;
  }
  return InMsg[0];
}


 void transmit(int message){  //Call with message to be transmitted
  OutMsg[0] = message;
  radioTransmit.write(OutMsg, 1);
}

int MessageCreation(int InterState, int Direction){
  int FinalMessage = CarID*8 + InterState*4 + Direction; //makes the binary version of the required message breaks if inproper values are used
  return FinalMessage;
}


 void InterMessage(int message){
  OtherCarMessage.CarID = (message%64)/8;
  OtherCarMessage.CarState = (message%8)/4;
  OtherCarMessage.CarDirection = message%4;
}
