
/*****************************************************************************
 *                                 Includes
 ****************************************************************************/
#include <SoftwareSerial.h>

 /*****************************************************************************
 *                                 TypeDefs
 ****************************************************************************/
typedef struct {
  int Speed;
  int Turn;
  
}DriveCommands;

typedef enum {
  
  Stopped = 000,
  Proceed= 001,
  Clear = 010,
  NotNear = 011,
  Blocked = 100,
   
}IntersectionState;

typedef struct {
  
 IntersectionState State;
 
}FSMVarsInt;

typedef struct {

  int CarID;
  int CarState;
  int CarDirection;
}MESSAGE;
/*****************************************************************************
 *                                 Defines
 ****************************************************************************/

 /*****************************************************************************
 *                             Function Prototypes
 ****************************************************************************/
 void InitComms();
 DriveCommands GetDriveCommands();
 void transmit(int message); 
 int receive();
 MESSAGE InterMessage(int NEWMessage);
 int MessageCreation(int InterState, int Direction);
