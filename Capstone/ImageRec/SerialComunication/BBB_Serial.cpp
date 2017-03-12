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

	Ret= LS.Open(DEVICE_PORT,9600);
	if (Ret!=1){
	  cout<<"cant open port\n";
	  return 0;
	}
	cout<<"port now open \n";

	Ret=LS.WriteString("Hello");                                             
	if (Ret!=1) {                                                           
		cout<<"Error while writing data\n";                              
		return Ret;                                                         
	}
	cout<<"Write operation is successful \n";

	LS.Close();
	cout<<"Transmission complete\n";
}
