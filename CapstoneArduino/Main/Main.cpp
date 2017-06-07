
/*****************************************************************************
 *                                 Includes
 ****************************************************************************/
#include "Main.h"
#include "Arduino.h"

 /*****************************************************************************
 *                             Global Variables
 ****************************************************************************/

FSMVars FSM;
Car CarPark[4] = {};
unsigned long CurrentTime;
unsigned long StartTime;
unsigned long ProceedStartTime;
unsigned long CurrentProceedTime;
unsigned long TimingThreshold;
int Priority;
int flag;

 /*****************************************************************************
 *                                 Functions
 ****************************************************************************/
 void InitMain(){
  
 
  FSM.State = STANDBY;
  FSM.InterState = NOTNEAR;
  FSM.CurrentSpeed = 0;
  FSM.CurrentServoPos = 0;
  FSM.DC.Speed = 0;
  FSM.DC.Turn = 0;
  FSM.DC.Direction = 0;
  FSM.SB = NODETECT;
  FSM.ErrorFlag = 0;
  FSM.InMessage.CarID = 0;
  FSM.CarPriority = 0;
  TimingThreshold = 2000;
  InitDrive();
  InitComms();
  InitSensors();
 }

 void DoStandByState(){
  SetSpeed(0);
  SetSteering(18);

  if(!FSM.ErrorFlag){
    FSM.State = PREDRIVE;
  }
  
 }

 void DoPreDriveState(){
  //FSM.SB = getSensorBehaviour();
  FSM.DC = GetDriveCommands();
  
  if(FSM.SB == NODETECT && FSM.DC.Speed > 0){
    SetSpeed(1);
    SetSteering(FSM.DC.Turn);
    FSM.CurrentSpeed = FSM.DC.Speed;
    FSM.CurrentServoPos = FSM.DC.Turn;
    FSM.State = DRIVE;
    
  }
 }
 
 void DoDriveState(){
  //Serial.println("Drive");
  FSM.SB = getSensorBehaviour();
  FSM.DC = GetDriveCommands();

  if(FSM.SB != NODETECT){
    SetSpeed(0);
    FSM.CurrentSpeed = 0;
    FlushSerial();
    Serial.println("Detected1");
  }


  else{
    SetSpeed(FSM.DC.Speed);
    SetSteering(FSM.DC.Turn);
    FSM.CurrentSpeed = FSM.DC.Speed;
    FSM.CurrentServoPos = FSM.DC.Turn;
    //Serial.println(FSM.DC.Speed);
    Serial.println(FSM.DC.Turn);
    Serial.println(FSM.DC.Direction);
    Serial.println(" ");
  }

  if(FSM.DC.Direction != 0){
    Serial.println("Stopping");
    ProceedStartTime = millis();
    CurrentProceedTime = millis();
    TimingThreshold = 1500;
    FSM.CarPriority = FSM.DC.Direction;

    while((CurrentProceedTime - ProceedStartTime) <= TimingThreshold){
      CurrentProceedTime = millis();
      FSM.SB = getSensorBehaviour();

      if(FSM.SB != NODETECT){
        Serial.println("Detected2");
        SetSpeed(0);
        if((CurrentProceedTime - ProceedStartTime) >= 500){
          ProceedStartTime = millis();
          CurrentProceedTime = millis();          
        }
         
      }
      
      else{
        SetSpeed(1);
      }
      
    }
    Serial.println("Interwait");

    FSM.State = INTERWAIT;
    FSM.InterState = STOPPED;
    StartTime = millis();
    SetSpeed(0);
    SetSteering(18);
    FSM.CurrentSpeed = 0;
    FSM.CurrentServoPos = 18;
    FlushSerial();
  }
  
  
  
 }

 void DoInterWaitState(){
  
  unsigned long Diff;

  Priority = 1;

  CurrentTime = millis();

  //Checks time since stopping at intersection
  Diff = CurrentTime - StartTime;

  
  //continously transmit car state
  transmit(FSM.InterState, FSM.DC.Direction);

  //check for messages
  for(int k = 0; k < 5; k++){
    FSM.InMessage = receive();

    if(FSM.InMessage.CarID != 0){
      break;
    }
  }
  
  
  //if non zero message is received run
  if(FSM.InMessage.CarID != 0 ){

    //if the message is a Proceed message run
    if(FSM.InMessage.CarState == PROCEED){
      
      //if the car is in the direction can go instantly
      if(FSM.InMessage.CarDirection == FSM.CarPriority){
        
        FSM.InterState = PROCEED;
        GoThruIntersectBBY();
        
      }

      //if the car is in the opposite direction intersection is blocked
      else{
        FSM.InterState = BLOCKED; 
        waitBitch(); 
      }    
    }


    //if the message isn't proceed add to message array
    else{
      for( int i = 0 ; i < 4; i++){

        //if the ID matches one already in array update that array element
        if(CarPark[i].ID == FSM.InMessage.CarID){
        
          CarPark[i].Direction = FSM.InMessage.CarDirection;
          CarPark[i].CarState = FSM.InMessage.CarState;
        }

        //if you reach a zero element in the array then add element to array
        else if(CarPark[i].ID == 0){
          CarPark[i].ID = FSM.InMessage.CarID;
          CarPark[i].Direction = FSM.InMessage.CarDirection;
          CarPark[i].CarState = FSM.InMessage.CarState;
        }    
      }
    }
  }

    //check to make sure car has waited for over 2 seconds
    if(Diff >= 1500){
      Serial.println("Timeout at intersection");
      //check through all elements of the array
      for( int j = 0; j < 4; j++){
       
        //if an array element is 0
        if(CarPark[j].ID == 0 || CarPark[3].ID != 0 ){

          //if the first array element is 0 then no other cars are near therefore go
          if(j == 0){
            
            FSM.InterState = PROCEED;
            GoThruIntersectBBY();
            break;
          }

          //occurs when all elements of array have been processed therefore decide what to do
          else{
            if(Priority == FSM.CarPriority){
              FSM.InterState = PROCEED;
              GoThruIntersectBBY();
            }
            else{
              FSM.InterState = BLOCKED;
              waitBitch();
            }
            break;
            
          }
        }
        
        //if non zero array element check state
        else{
          
          if(CarPark[j].CarState == STOPPED){
            if(CarPark[j].Direction == 2){
              Priority = 2;
            }                
          }        
        }
      }
    }   
  }

  void GoThruIntersectBBY(){

    Serial.println("Proceeding");
    
    ProceedStartTime = millis();
    CurrentProceedTime = millis();
    TimingThreshold = 2000;

    transmit(FSM.InterState, FSM.DC.Direction);
    SetSpeed(1);
    SetSteering(18);

    while((CurrentProceedTime - ProceedStartTime) <= TimingThreshold){
      CurrentProceedTime = millis();
      FSM.SB = getSensorBehaviour();

      if(FSM.SB != NODETECT){
        Serial.println("Detected3");
        SetSpeed(0);
        if((CurrentProceedTime - ProceedStartTime) >= 900){
          ProceedStartTime = millis();
          CurrentProceedTime = millis();
          TimingThreshold = 1000;          
        }
         
      }
      
      else{
        SetSpeed(1);
      }
    
   
    }
    
    FSM.InterState = CLEAR;
    transmit(FSM.InterState, FSM.DC.Direction);
    FSM.DC.Direction = 0;
    FSM.InterState = NOTNEAR;
    FSM.State = DRIVE;
    TimingThreshold = 2000;
    for(int p = 0; p <= 5; p++){
      PingBB();
    }
    Serial.println("Pinged");
    PingBB();
    memset(CarPark,0,4);
    
  }
  
   void waitBitch(){
    Serial.println("waiting passive-aggressively");
    unsigned long WaitTime = millis();
    unsigned long PassTime = millis();
    while((WaitTime - PassTime) < 2000){
      FSM.InMessage = receive();
      PassTime = millis();
      if(FSM.InMessage.CarState == CLEAR){
        FSM.InterState = PROCEED;
        GoThruIntersectBBY();
      }
    }
    FSM.InterState = PROCEED;
    GoThruIntersectBBY();
   }
 

 void DoErrorState(){
  FSM.State = STANDBY;
 }

 void ExecuteFSM(){
  switch (FSM.State){
    
    case STANDBY:
      DoStandByState();
    break;

    case PREDRIVE:
      DoPreDriveState();
    break;

    case DRIVE:
      DoDriveState();
    break;

    case INTERWAIT:
      DoInterWaitState();
    break;

    case ERRORS:
      DoErrorState();
    break;

    default:
    break;
  }
 }


 void Main(){
  ExecuteFSM();
 }

 void Reset(){
  InitMain();
 }

