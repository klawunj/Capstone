
#ifndef INCLUDES_MAIN_H_
#define INCLUDES_MAIN_H_

/*****************************************************************************
 *                                 Includes
 ****************************************************************************/
 #include "Drive.h"
 #include "Comms.h"

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
 float CurrentSpeed;
 float CurrentServoPos;
 
}FSMVars;

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

#endif /* INCLUDES_MAIN_H_ */

