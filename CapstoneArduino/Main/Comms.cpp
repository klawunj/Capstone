/*****************************************************************************
 *                                 Includes
 ****************************************************************************/
#include "Comms.h"
#include "Arduino.h"
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
 

 /*****************************************************************************
 *                                 Functions
 ****************************************************************************/

 void InitComms(){
  Serial2.begin(9600);
  radioRecieve.begin();
  radioTransmit.begin();
  radioRecieve.openReadingPipe(1,pipe2);
  radioRecieve.startListening();
  radioTransmit.openWritingPipe(pipe1);
  InMsg[0] = 0;
  OutMsg[0] = 0;
 }

void PingBB(){
  Serial2.write("\n");
}

DriveCommands GetDriveCommands(){
  //output
  DriveCommands output;
  //Variables
  String inputBuffer;//variable to get the serial value
  int conIB;//store the integer value of the serial value
  String Speed = "";//store the integer speed value
  String Turn = "";//store the turn value
  String Direction = "";
  int intSpeed, intTurn, intDirec;
  int variable = 0;//signifies the switch between speed and turn when reading values
  
  //Flags and checks
  bool flag = 0;//ensure that full message is only printed once
  bool ping = 1;

  while (1){

    if(ping == 1){
      PingBB();
    }
    
    if(Serial2.available()){
      inputBuffer = Serial2.read();
      ping = 0;

      conIB = inputBuffer.toInt();
      conIB = conIB - 48; //convert ascii values to usable numbers

       //If there is a space, switch which variable written to
      if(conIB == -16){
        
        if(variable == 0){
          variable = 1;
        }
        
        else{
          variable = 2;
        }
        
      }

       //If there is #, means end of message
      else if (conIB == -13){
        //check if the first time sent
        if (flag == 0){
          intSpeed = Speed.toInt();
          intTurn = Turn.toInt();
          intDirec = Direction.toInt();
          output.Speed = intSpeed;
          output.Turn = intTurn;
          output.Direction = intDirec;

          //return value
          return output;
        }
      }

       //Add the number value to the message
      else{
        if(conIB >= 0){
          if (variable == 0){
            Speed = Speed+conIB;
          }
        
          else if(variable == 1){
            Turn = Turn+conIB;
          }

          else{
            Direction = Direction + conIB;
          }
        
          flag = 0;
        }
      }
    }
 }
}


  MESSAGE receive(){  //Recieves message and returns value
  
  MESSAGE IncomingMessage;
   
  int InState;
   
  if (radioReceive.available()){
    while (radioReceive.available()){
      radioReceive.read(InMsg, 1);     
    }
  }

  else{
    //Serial.println("Not Available");
    InMsg[0] = 0;
  }
  
  IncomingMessage.CarID = (InMsg[0]%128)/16;
  InState = (InMsg[0]%16)/4;
  IncomingMessage.CarDirection = InMsg[0]%4;
 
  
  if(InState == 1){
    IncomingMessage.CarState = PROCEED;
  }

  else if(InState == 2){
    IncomingMessage.CarState = CLEAR;
  }

  else if(InState == 3){
    IncomingMessage.CarState = STOPPED;  
  }

  else{
    IncomingMessage.CarState = NOTNEAR;
  }
  
  
  return IncomingMessage;
}


 void transmit(int InterState, int Direction){  //Call with message to be transmitted
  
  int FinalMessage = CarId*16 + InterState*4 + Direction; 
  OutMsg[0] = FinalMessage;
  //Serial.println(OutMsg[0]);
  radioTransmit.write(OutMsg, 1);
 
  
}


