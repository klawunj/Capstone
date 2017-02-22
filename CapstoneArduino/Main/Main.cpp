
/*****************************************************************************
 *                                 Includes
 ****************************************************************************/
#include "Main.h"

 /*****************************************************************************
 *                             Global Variables
 ****************************************************************************/

FSMVars FSM;

 /*****************************************************************************
 *                                 Functions
 ****************************************************************************/
 void InitMain(){
  FSM.State = STANDBY;
  FSM.CurrentSpeed = 0;
  FSM.CurrentServoPos = 0;
  InitDrive();
  InitComms();
 }

 void DoStandByState(){
  
 }

 void DoPreDriveState(){
  
 }
 
 void DoDriveState(){
  
 }

 void DoInterWaitState(){
  
 }

 void DoErrorState(){
  
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

