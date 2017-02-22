#ifndef INCLUDES_DRIVE_H_
#define INCLUDES_DRIVE_H_

/*****************************************************************************
 *                                 Includes
 ****************************************************************************/
#include <Servo.h>
/*****************************************************************************
 *                                 TypeDefs
 ****************************************************************************/

/*****************************************************************************
 *                                 Defines
 ****************************************************************************/
#define motorpin 9
#define servopin 10
#define MaxSpeed 90
#define MinSpeed 0
#define MaxRightServoPos 10
#define MaxLeftServoPos -10
/*****************************************************************************
 *                             Function Prototypes
 ****************************************************************************/
 void InitDrive();
 void SetSpeed(int UnMappedSpeed);
 void SetSteering(int UnMappedServoPos);
 void SteerControl(int NewServoPos);
 




#endif /* INCLUDES_DRIVE_H_ */
