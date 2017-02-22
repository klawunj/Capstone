/*****************************************************************************
 *                                 Includes
 ****************************************************************************/

#include "Drive.h"

/*****************************************************************************
 *                             Global Variables
 ****************************************************************************/

Servo Motor;
Servo Steering;

/*****************************************************************************
 *                                 Functions
 ****************************************************************************/

 void InitDrive(){
  
  Motor.attach(motorpin);
  Steering.attach(servopin);

  SetSpeed(0);
  SetSteering(0);

 }
 
//speed must be value from 0 - 90
 void SetSpeed(int UnMappedSpeed){

   int MappedSpeed;
  
   if (UnMappedSpeed > MaxSpeed){
    UnMappedSpeed = MaxSpeed;
   }

   if (UnMappedSpeed < MinSpeed){
    UnMappedSpeed = MinSpeed;
   }

   MappedSpeed = 90 - UnMappedSpeed;
   
   Motor.write(MappedSpeed);

 }

// servo pos must be value from -10 - +10
 void SetSteering(int UnMappedServoPos){

  int MappedServoPos;

  if (UnMappedServoPos > MaxRightServoPos){
    UnMappedServoPos = MaxRightServoPos;
  }

  if (UnMappedServoPos < MaxLeftServoPos){
    UnMappedServoPos = MaxLeftServoPos;
  }

  MappedServoPos = UnMappedServoPos + 30;

  Steering.write(MappedServoPos);
  
 }

 void SteerControl(int NewServoPos){
  
 }

