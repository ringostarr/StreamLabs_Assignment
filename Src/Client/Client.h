#pragma once
/////////////////////////////////////////////////////////////////////
// Client.h - Client header for named pipes                        //
//  Akshay,                                                        //
//  3467,Peralta Blvd , Fremont , California - 94536.              //
//  Akshay@syr.edu                                                 //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Precision T7400, Windows 10 Ultimate       //
//  Application:   Named Pipes Take home assignment for StreamLabs //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
Header for all client functionality , including Sync/Async send/recv, processing received messages

Public Interface:
=================
Class Client
-CreatePipe() - Connect to Server using Createfile function Named pipes windows API , returns false if error.
-isPipeValid() - Checks if Pipe is valid
-ClosePipe() - Closes current pipe connection using closeHandle function provided by Windows API .
-processReply() - processes the reply received from the server
-sendMsg() - decides if the msg is sync or async and calls the appropriate function
Private:
-SendAsync() - Asynchronous sending function.
-SendSync() - Synchronous sending function
Build Process:
==============
* Required Files :Utilities.h , Client.h , CustomClass.h,Client.cpp


Build commands
- devenv Utilities.h,Client.h


Maintenance History:
====================
ver1.0 : Initial commit
*/



#include<iostream>
#include<string>
#include <vector>
#include<Windows.h>
#include "../Utilities/Utilities.h"
#include "../Customclass/Customclass.h"
#define CONNECTING_STATE 0 
#define READING_STATE 1 
#define WRITING_STATE 2 

#define lpszPipename "\\\\.\\pipe\\ServerPipe"

typedef struct
{
	OVERLAPPED oOverlap;
	HANDLE hPipeInst;
	TCHAR chRequest[4096];
	DWORD cbRead;
	TCHAR chReply[4096];
	DWORD cbToWrite;
	DWORD dwState;
	BOOL fPendingIO;
} PIPEINST, *LPPIPEINST;

class Client {
public:
	//Pipe creation
	int CreatePipe()
	{
		std::cout << "Connecting to Server\n";
		Cpipe = CreateFile(TEXT(lpszPipename), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if (Cpipe == NULL && Cpipe == INVALID_HANDLE_VALUE)
		{
			std::cout << "FAILED TO CONNECT TO SERVER\n";
			return false;
		}
		return true;
	}
	//check if pipe exists
	bool isPipeValid()
	{
		return (Cpipe != NULL && Cpipe != INVALID_HANDLE_VALUE);
	}
	//close pipe
	int closepipe()
	{
		std::cout << "Closing Pipe Handle\n";
		return CloseHandle(Cpipe);
	}

	// check if msg to be sent has async flag or not ,i.e "-a" at the end of the message
	bool isMsgAsync(std::string &);

	//send Msg to Async or Sync after checking async flag
	void sendMsg(std::string);

	//process reply received from server
	void processReply(std::string);

private:
	HANDLE Cpipe;

	//Async sending and receiving
	void sendAsync(std::string &);

	//Sync Sending and receiving
	void sendSync(std::string &);




};
bool Client::isMsgAsync(std::string &msg)
{
	if (msg.substr(msg.size() - 2, msg.size() - 1) == "-a")
	{
		msg = msg.substr(0, msg.size() - 2);
		return true;
	}
	else
		return false;
}
void Client::sendMsg(std::string Msg)
{
	if (isMsgAsync(Msg))
		sendAsync(Msg);
	else
		sendSync(Msg);
}

void Client::sendAsync(std::string &msgsend)
{

	std::cout << "\nAsync Send function called ! \n";
	OVERLAPPED _write, _read;
	DWORD numbytes = 0;
	memset(&_write, 0, sizeof(_write));
	memset(&_read, 0, sizeof(_read));
	_write.hEvent = CreateEvent(NULL, true, true, NULL);
	_read.hEvent = CreateEvent(NULL, true, true, NULL);
	if (_write.hEvent == NULL || _read.hEvent == NULL)
	{
		std::cout << "Create Event failed , err : " << GetLastError() << std::endl;
		return;
	}
	int error = 0;
	//write logic
	bool success;
	if (!isPipeValid())
	{
		std::cout << "Invalid Pipe :Please Create Pipe first." << std::endl;
		return;
	}
	success = WriteFile(Cpipe, (void*)msgsend.c_str(), sizeof(msgsend), NULL, &_write);
	error = GetLastError();
	if (error == ERROR_BROKEN_PIPE)
	{
		std::cout << "Pipe is broken " << std::endl;
		return;
	}
	if (!success && error == ERROR_IO_PENDING) // IO is still not done
	{
		if (WaitForSingleObject(_write.hEvent, INFINITE) == WAIT_OBJECT_0) // wait
		{
			GetOverlappedResult(Cpipe, &_write, &numbytes, NULL);
			std::cout << "Async Write Operation done ! " << std::endl;
		}
		else
		{
			//Cancel IO since wait failed or returned a different error like timeout etc.
			CancelIo(Cpipe);
			std::cout << "Write operation cancelled due to error " << std::endl;
			return;
		}
	}
	else if (success)
	{
		std::cout << "\nAsync write done ! No need to wait! \n" << std::endl;
	}
	char buffer[4096];
	//Reading logic
	if (!isPipeValid())
	{
		std::cout << "Invalid Pipe :Please Create Pipe first." << std::endl;
		return;
	}
	success = ReadFile(Cpipe, (void*)buffer, 4096 * sizeof(char), 0, &_read);
	error = GetLastError();
	if (error == ERROR_BROKEN_PIPE)
	{
		std::cout << "Broken Pipe " << std::endl;
		return;
	}
	if (!success && error == ERROR_IO_PENDING)
	{
		if (WaitForSingleObject(_read.hEvent, INFINITE) == WAIT_OBJECT_0)
		{
			GetOverlappedResult(Cpipe, &_read, &numbytes, true);
			std::cout << "\nAsync read operation completed after waiting\n" << std::endl;
		}
		else
		{
			CancelIo(Cpipe);
			std::cout << "Async Read error!" << std::endl;
			return;
		}
	}
	else if (success)
	{
		std::cout << "\nAsync Read completed ,new msg received \n ";

		processReply(buffer);
	}
}


void Client::sendSync(std::string &msgsend)
{

	DWORD numbytes;

	char buffer[4096];
	if (!isPipeValid())
	{
		std::cout << "Invalid Pipe :Please Create Pipe first." << std::endl;
		return;
	}
	bool success = WriteFile(Cpipe, (void*)msgsend.c_str(), sizeof(msgsend), &numbytes, NULL);
	std::cout << "\nMsg sent from client : " << msgsend << std::endl;
	if (!isPipeValid())
	{
		std::cout << "Invalid Pipe :Please Create Pipe first." << std::endl;
		return;
	}
	success = ReadFile(Cpipe, (void*)buffer, 4096 * sizeof(char), 0, NULL);
	if (!success) {
		std::cout << "\nError while reading \n";
		return;
	}
	std::cout << "\nNew reply recieved from server\n";
	//msgrecv = buffer;
	processReply(buffer);

}


//Process the reply
void Client::processReply(std::string msg)
{
	std::cout << " \n Processing Msg! " << std::endl;
	std::vector<std::string> parts = splitString(msg, "$#^");
	if (parts[0] == "Int")
		std::cout << "\nReceived Integer : " << std::stoi(parts[1]);
	else if (parts[0] == "Str")
		std::cout << "\nReceived String : " << parts[1];
	else if (parts[0] == "ObjAttr")
	{
		std::cout << "\nRetreving object\n";
		Customclass CustObj(std::stoi(parts[1]), parts[2]);
		std::cout << "\n------Object Attributes----\n";
		std::cout << "--UID : " << CustObj.getuid();
		std::cout << "\n--Name :" << CustObj.getname();
		std::cout << "\n Obj can now be saved in map and used later (not implemented)";
		std::cout << "\n----End of Msg----\n";
	}
	else if (parts[0] == "FnOut")
		std::cout << "\nSerialized Function Output : " << parts[1];
	else
		std::cout << std::endl << "Recieved Msg : " << parts[0];

	std::cout << "\nEnd of Processing Reply\n";

}
