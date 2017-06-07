#ifndef INCLUDES_COMMS_H_
#define INCLUDES_COMMS_H_

/*****************************************************************************
 *                                 Includes
 ****************************************************************************/

 /*****************************************************************************
 *                                 TypeDefs
 ****************************************************************************/
typedef struct {
  int Speed;
  int Turn;
  int Direction;
  
}DriveCommands;

typedef enum {
  
  NOTNEAR = 0,
  PROCEED = 1,
  CLEAR = 2,
  STOPPED = 3,
  BLOCKED = 4
   
}IntersectionState;


typedef struct {

  int CarID;
  IntersectionState CarState;
  int CarDirection;
}MESSAGE;
/*****************************************************************************
 *                                 Defines
 ****************************************************************************/
#define CarId 2
 /*****************************************************************************
 *                             Function Prototypes
 ****************************************************************************/
 void InitComms();
 DriveCommands GetDriveCommands();
 void PingBB();
 void transmit(int InterState, int Direction); 
 MESSAGE receive();
 void FlushSerial();

#endif
