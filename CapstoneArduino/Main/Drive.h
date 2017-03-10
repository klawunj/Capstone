#ifndef INCLUDES_DRIVE_H_
#define INCLUDES_DRIVE_H_

/*****************************************************************************
 *                                 Includes
 ****************************************************************************/
#include <Servo.h>
/*****************************************************************************
 *                                 TypeDefs
 ****************************************************************************/
 
 typedef struct{
  float looptime;
  float Ki;
  float Kp;
  float Kd;
  float previousError;
  float previousIntegral;
  int StartValue;
  int OldSetpoint;
   
 }PIController;

 typedef enum{
  SPEED = 0,
  TURN = 1
  
 }ControlMode;


/*****************************************************************************
 *                                 Defines
 ****************************************************************************/
#define motorpin 9
#define servopin 10
#define MaxSpeed 45
#define MinSpeed 0
#define MaxRightServoPos 36
#define MaxLeftServoPos 0
/*****************************************************************************
 *                             Function Prototypes
 ****************************************************************************/
 void InitDrive();
 void InitPIControllers();
 void SetSpeed(int UnMappedSpeed);
 void SetSteering(int UnMappedServoPos);
 int DriveControl(int NewVal, int CurrentVal, ControlMode Mode);
 




#endif /* INCLUDES_DRIVE_H_ */
