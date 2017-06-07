
#ifndef INCLUDES_MAIN_H_
#define INCLUDES_MAIN_H_

/*****************************************************************************
 *                                 Includes
 ****************************************************************************/
 #include "Drive.h"
 #include "Comms.h"
 #include "Sensors.h"

/*****************************************************************************
 *                                 TypeDefs
 ****************************************************************************/
typedef enum {
  
  STANDBY = 0,
  PREDRIVE = 1,
  DRIVE = 2,
  INTERWAIT = 3,
  ERRORS = 4
  
}STATE;

typedef struct {
  
 STATE State;
 IntersectionState InterState;
 DriveCommands DC;
 SensorBehaviour SB;
 int CurrentSpeed;
 int CurrentServoPos;
 MESSAGE InMessage;
 int OutMessage;
 int ErrorFlag;
 int CarPriority;

 
}FSMVars;

typedef struct{
  int ID;
  int Direction;
  IntersectionState CarState;
  
}Car;



/*****************************************************************************
 *                                 Defines
 ****************************************************************************/

/*****************************************************************************
 *                             Function Prototypes
 ****************************************************************************/

void InitMain();
void Main();
void ExecuteFSM();
void DoStandByState();
void DoPreDriveState();
void DoDriveState();
void InterWaitState();
void DoErrorState();
void GoThruIntersectBBY();
void waitBitch();
void Reset();

#endif /* INCLUDES_MAIN_H_ */

