// client.cpp : Defines the entry point for the console application.
//


#include <iostream>
#include <windows.h>
#include<string>
#include "Client.h"

using namespace std;

int main(int argc, const char **argv)
{


	Client c2;
	//std::string msg = "addobj$#^1$#^123$#^Akshay", msgrevd;
	//c2.CreatePipe();
	c2.CreatePipe();
	std::cout << "\n-----------Testing ------\n";
	std::cout << "----------- TEST 1-------------\n ";
	std::cout << "Sending Integer to server with key 1 and value 22\n";
	std::string IntMsg = "addint$#^1$#^22";
	//std::string RecvMsg;
	c2.sendMsg(IntMsg);
	std::cout << "--------End of TEST 1---------\n";
	std::cout << "----------TEST2-----------------\n";
	std::cout << "\n\n Sending String with key 1 and value abcd\n";
	std::string StrMsg = "addstr$#^1$#^abcd";
	c2.sendMsg(StrMsg);
	std::cout << "-----------End of TEST 2-------------\n";
	std::cout << "--------------TEST 3 -----------------\n";
	std::cout << "\n\n Sending CustomObj with key 1 , uid: 121 and name:Testing OBJ sending";
	std::string ObjMsg = "addobj$#^1$#^121$#^Test,OBJ";
	c2.sendMsg(ObjMsg);
	std::cout << "\n--------------End of TEST 3-------------";
	std::cout << "\n---------------TEST 4 -------------------\n";
	std::cout << "\n\nTrying to retrieve integer with key 1 \n";
	std::string getintmsg = "getint$#^1";
	c2.sendMsg(getintmsg);
	std::cout << "-----------End of TEST 4-------------\n";

	std::cout << "\n-----------TEST 5 ------------------\n";
	std::cout << "\n Trying to retrieve string with key 1 using asynchronous send\n";
	std::string getstrMsg = "getstr$#^1-a";
	c2.sendMsg(getstrMsg);
	std::cout << "\n-------------End of TEST 5 --------------\n";
	std::cout << "\n-------------TEST 6--------------------\n";
	std::cout << "\nTrying to retrieve object with key 1 \n";
	std::string getObjMsg = "getobj$#^1";
	c2.sendMsg(getObjMsg);
	std::cout << "\n---------------END OF TEST 6 -----------\n";
	std::cout << "\n----------------TEST 7 -----------------\n";
	std::cout << " Trying to call getname function on obj with key 1\n";
	std::string callObjFn = "callfunction$#^1$#^getname";
	c2.sendMsg(callObjFn);
	std::cout << "\n---------------End of TEST 7 ----------------\n";
	std::string MsgtoSend;
	std::cout << "Please enter Message to be sent : \n";
	do
	{
		//std::string MsgtoSend;
		cin >> MsgtoSend;
		if (MsgtoSend == "exit")
			break;
		c2.sendMsg(MsgtoSend);
	} while (MsgtoSend != "exit");
	c2.closepipe();

	return 0;


	//return 0;
}