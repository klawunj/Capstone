/*****************************************************************************
 *                                 Includes
 ****************************************************************************/

#include "Drive.h"

/*****************************************************************************
 *                             Global Variables
 ****************************************************************************/

Servo Motor;
Servo Steering;
PIController SpeedController;
PIController SteeringController;

/*****************************************************************************
 *                                 Functions
 ****************************************************************************/

 void InitDrive(){

  InitPIControllers();
  Motor.attach(motorpin);
  Steering.attach(servopin);

  SetSpeed(0);
  SetSteering(0);

 }

 void InitPIControllers(){
  SpeedController.Kp = 0.17;   //Increasing 0.17
  SpeedController.Ki = 0.5;   // Increasing 0.5
  SpeedController.Kd = 0.01;  // Increasing 0.01
  SpeedController.looptime = 0.1;
  SpeedController.previousError = 0;
  SpeedController.previousIntegral = 0;
  SpeedController.StartValue = 0;
  SpeedController.OldSetpoint = 0;

  SteeringController.Kp = 0.2;
  SteeringController.Ki = 0.5;
  SteeringController.Kd = 0.01;
  SteeringController.looptime = 0.1;
  SteeringController.previousError = 0;
  SteeringController.previousIntegral = 0;
  SteeringController.StartValue = 0;
  SteeringController.OldSetpoint = 0;
  
 }
 
//speed must be value from 0 - 45
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

// servo pos must be value from 0 - 20
 void SetSteering(int UnMappedServoPos){

  int MappedServoPos;

  if (UnMappedServoPos > MaxRightServoPos){
    UnMappedServoPos = MaxRightServoPos;
  }

  if (UnMappedServoPos < MaxLeftServoPos){
    UnMappedServoPos = MaxLeftServoPos;
  }

  MappedServoPos = UnMappedServoPos + 20; //conversion factor actual center is 30

  Steering.write(MappedServoPos);
  
 }

//if mode = 0 speed control, if mode = 1 steering control
 int DriveControl(int NewVal, int CurrentVal, ControlMode Mode){

  int error;
  float KpTerm;
  float KiTerm;
  float KdTerm;
  float der;
  int output;

 
  if((NewVal - CurrentVal) < 0){
    if(Mode == SPEED){
      
      if(NewVal != SpeedController.OldSetpoint){
          SpeedController.StartValue = (90 - CurrentVal);
      }
      
      error = (90 - NewVal) - (90 - CurrentVal);
    
    }

    else{
      
      if(NewVal != SteeringController.OldSetpoint){
          SteeringController.StartValue = (20 - CurrentVal);
      }

      error = (20 - NewVal) - (20 - CurrentVal);
    }
  }

  else{
    if(Mode == SPEED){
      if(NewVal != SpeedController.OldSetpoint){
        SpeedController.StartValue = CurrentVal;
      }
    }

    else{
      if(NewVal != SteeringController.OldSetpoint){
        SteeringController.StartValue = CurrentVal;
      } 
    }

    
    error = NewVal - CurrentVal;  
    
  }
  if(error >= 3 || error <= -3){
    if(Mode == SPEED){
      SpeedController.previousIntegral += error*SpeedController.looptime;
      der = (error - SpeedController.previousError)/SpeedController.looptime;

      KpTerm = SpeedController.Kp*error;
      KiTerm = SpeedController.Ki*SpeedController.previousIntegral;
      KdTerm = SpeedController.Kd*der;
      
      output = SpeedController.StartValue + KpTerm + KiTerm + KdTerm;

      SpeedController.previousError = error;
      
      if((NewVal - CurrentVal) < 0){

        if((90 - output) <= 4){
          output = 0;
        }
        
        else{
          output = (90 - output);
        }
        
        SetSpeed(output);
        return output;
        
      }
      
      else{
        
        if(output <= 4){
          output = 0;
        }

        SetSpeed(output);
        return output;
      }
    
    }
    
    else{
      SteeringController.previousIntegral += error*SteeringController.looptime;
      der = (error - SteeringController.previousError)/SteeringController.looptime;

      KpTerm = SteeringController.Kp*error;
      KiTerm = SteeringController.Ki*SteeringController.previousIntegral;
      KdTerm = SteeringController.Kd*der;
      
      output = SteeringController.StartValue + KpTerm + KiTerm + KdTerm;

      SpeedController.previousError = error;
   

      if((NewVal - CurrentVal) < 0){

        output = (20 - output);

        SetSteering(output);
        return output;
        
      }
      
      else{

        SetSteering(output);
        return output;
      }
    }
  }
 
 }

