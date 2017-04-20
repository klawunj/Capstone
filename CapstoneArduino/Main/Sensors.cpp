/*****************************************************************************
 *                                 Includes
 ****************************************************************************/
#include "Sensors.h"
 /*****************************************************************************
 *                             Global Variables
 ****************************************************************************/
 int count;

  /*****************************************************************************
 *                                 Functions
 ****************************************************************************/

void InitSensors(){
  pinMode(IRPinR, INPUT);
  pinMode(IRPinL, INPUT);
  count = 0;
 }

 SensorVals getIRValue(){
  SensorVals SV;
  unsigned int readingR = analogRead(IRPinR);
  unsigned int readingL = analogRead(IRPinL);

  SV.rSensor = readingR;
  SV.lSensor = readingL;
  
  return SV;
  
 }

 SensorBehaviour getSensorBehaviour(){
  SensorBehaviour output;
  
  for(int i = 0; i < 3; i++){
    SensorVals input = getIRValue();
    
    if(input.rSensor >= IRStopThresh || input.lSensor >= IRStopThresh){
      count++;
    }
    
  }
    

  if(count >= 2){
    output = STOP;
  }
  //useless code below since code reads anything other than stop as keep going, change if we implement more speeds
  else if(count == 1){
    output = SLOWDOWN;
  }

  else{
    output = NODETECT;
  }
  count = 0;
  return output;

 }
