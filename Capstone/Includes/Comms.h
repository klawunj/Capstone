/*
 * Comms.h
 *
 *  Created on: Nov 18, 2016
 *      Author: Paul
 */

#ifndef INCLUDES_COMMS_H_
#define INCLUDES_COMMS_H_

/*****************************************************************************
 *                                 Includes
 ****************************************************************************/

/*****************************************************************************
 *                                 TypeDefs
 ****************************************************************************/
typedef enum{

	STOP = 0,
	ACKNOWLEDGE = 1,
	GOING = 2,
	ARRIVED = 3

}identifier;

typedef struct{

	identifier ident;
	int queuePos;
	int intersectionLock;
	unsigned short fullmessage; // place holder for datatype of messages


}message;

/*****************************************************************************
 *                                 Defines
 ****************************************************************************/

/*****************************************************************************
 *                             Function Prototypes
 ****************************************************************************/

void sendMessage(message m);
void processMessage(message m);
void getMessage();


#endif /* INCLUDES_COMMS_H_ */
