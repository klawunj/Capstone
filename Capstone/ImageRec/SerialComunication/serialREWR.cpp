#include <iostream>
#include "serialib.h"

#ifdef __linux__
#define     DEVICE_PORT    "/dev/ttyS1"
#endif

using namespace std;
int main()
{
	serialib LS; //the main class to access
	int Ret;
	char Buffer[128];

	Ret= LS.Open(DEVICE_PORT,9600);
	if (Ret!=1){
	  cout<<"cant open port\n";
	  return 0;
	}
	cout<<"port now open \n";

	Ret=LS.ReadString(Buffer,'\n',128,5000);                                // Read a maximum of 128 characters with a timeout of 5 seconds
                                                                            // The final character of the string must be a line feed ('\n')
    if (Ret>0)                                                              // If a string has been read from, print the string
        cout<<"String read from serial port : " << Buffer <<"\n";
    else
        cout<<"TimeOut reached. No data received !\n";                   // If not, print a message.
	cout<< Buffer << endl;
	cout<< Ret << endl;
	
	Ret=LS.WriteString("0 36 1#");                                             
	if (Ret!=1) {                                                           
		cout<<"Error while writing data\n";                              
		return Ret;                                                         
	}
	cout<<"Write operation is successful \n";

	LS.Close();
	cout<<"Transmission complete\n";
}
