/*****************************************************************************
 *                                 Includes
 ****************************************************************************/
 #include "Arduino.h"

 /*****************************************************************************
 *                                 TypeDefs
 ****************************************************************************/
 typedef enum{
  NODETECT = 0,
  SLOWDOWN = 1,
  STOP = 2
  
 }SensorBehaviour;

 typedef struct{
  unsigned int rSensor;
  unsigned int lSensor;
  
 }SensorVals;

 /*****************************************************************************
 *                                 Defines
 ****************************************************************************/
#define IRSlowThresh 100
#define IRStopThresh 200
#define IRPinR A0
#define IRPinL A15
/*****************************************************************************
 *                             Function Prototypes
 ****************************************************************************/
 void InitSensors();
 SensorVals getIRValue();
 SensorBehaviour getSensorBehaviour();
