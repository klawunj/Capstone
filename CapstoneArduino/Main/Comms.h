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
  
  STOPPED = 0,
  PROCEED = 1,
  CLEAR = 2,
  NOTNEAR = 3,
  BLOCKED = 4
   
}IntersectionState;


typedef struct {

  int CarID;
  int CarState;
  int CarDirection;
}MESSAGE;
/*****************************************************************************
 *                                 Defines
 ****************************************************************************/
#define CarId 3
 /*****************************************************************************
 *                             Function Prototypes
 ****************************************************************************/
 void InitComms();
 DriveCommands GetDriveCommands();
 void PingBB();
 void transmit(int message); 
 int receive();
 MESSAGE InterMessage(int NEWMessage);
 int MessageCreation(int InterState, int Direction);

#endif
