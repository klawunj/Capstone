/*****************************************************************************
 *                                 Includes
 ****************************************************************************/
#include "Comms.h"
/*****************************************************************************
 *                             Global Variables
 ****************************************************************************/
 

 /*****************************************************************************
 *                                 Functions
 ****************************************************************************/

 void InitComms(){
  Serial.begin(9600);
  while(!Serial){
    
  }
 }

 DriveCommands GetDriveCommands(){
  String DriveCommand;
  
 if (Serial.available()){
    DriveCommand = Serial.read();
  }

  
  
 }
