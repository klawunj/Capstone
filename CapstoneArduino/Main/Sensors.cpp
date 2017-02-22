/*****************************************************************************
 *                                 Includes
 ****************************************************************************/
#include "Sensors.h"
 /*****************************************************************************
 *                             Global Variables
 ****************************************************************************/
 

  /*****************************************************************************
 *                                 Functions
 ****************************************************************************/

 void InitSensors(){
  pinMode(IRPin, INPUT);
 }

 int getIRValue(){
  unsigned int reading = analogRead(IRPin);
  return reading;
  
 }

 SensorBehaviour getSensorBehaviour(){
  SensorBehaviour output;
  int SensorVal = getIRValue();

  if(SensorVal >= IRStopThresh){
    output = STOP;
  }
  else if(SensorVal >= IRSlowThresh){
    output = SLOWDOWN;
  }

  else{
    output = NODETECT;
  }

  return output;

 }
