#pragma once

/////////////////////////////////////////////////////////////////////
// Server.h - Server header for named pipes                        //
//  Akshay,                                                        //
//  3467,Peralta Blvd , Fremont , California - 94536.              //
//  Akshay@syr.edu                                                 //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Precision T7400, Vista Ultimate SP1        //
//  Application:   Named Pipes Take home assignment for StreamLabs //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
Header for all Server functionality , including processing received messages

Public Interface:
=================
Class Server
-Start() - Create pipe and wait for connection
-Run() - Accepts connection and read/write to pipe
-ConnectToNewClient : Connects to a new client
-DisconnectnReconnect : Disconnect and reconnect
-addInt() - add Integer to database
-addStr() - add string to database
-addObj() - add custom object to database
-processRequest() - process incoming message and return reply

Build Process:
==============
* Required Files :Utilities.h , Server.h , CustomClass.h

Maintenance History:
====================
ver1.0 : Initial commit
*/


#include <unordered_map>
#include <string>
#include<Windows.h>
#include <iostream>
#include<tchar.h>
#include<strsafe.h>
#include<stdio.h>
#include "../CustomClass/CustomClass.h"
#include "../Utilities/Utilities.h"
#define lpszPipename "\\\\.\\pipe\\ServerPipe"

#define CONNECTING_STATE 0 
#define READING_STATE 1 
#define WRITING_STATE 2
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


HANDLE hEvent;
PIPEINST Pipe;


class Server
{
private:
	std::unordered_map<int, int> IntCache;
	std::unordered_map<int, std::string> StrCache;
	std::unordered_map<int, Customclass> ObjCache;
public:
	bool addInt(int, int);
	bool addStr(int, std::string);
	bool addObj(int, Customclass);
	std::string processRequest(std::string);

	//Create instnce of named pipe to start server
	int startServ();

	//Run Server
	int run();

	BOOL ConnectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo);
	VOID DisconnectnReconnect();

	VOID GetAnswerToRequest(LPPIPEINST pipe)
	{
		std::cout << std::endl << "Revd msg : " << Pipe.chRequest << std::endl;
		std::string resultout = processRequest(std::string(pipe->chRequest));

		StringCchCopy(pipe->chReply, 4096, resultout.c_str());
		pipe->cbToWrite = (lstrlen(pipe->chReply) + 1) * sizeof(TCHAR);
		memset(pipe->chRequest, 0, 4096);
	}

};


bool Server::addInt(int key, int val)
{
	if (IntCache.find(key) == IntCache.end())
	{
		IntCache.insert(std::make_pair(key, val));
		return true;
	}
	else
		return false;
}

bool Server::addStr(int key, std::string val)
{
	if (StrCache.find(key) == StrCache.end())
	{
		StrCache.insert(std::make_pair(key, val));
		return true;
	}
	else
		return false;
}

bool Server::addObj(int key, Customclass val)
{
	if (ObjCache.find(key) == ObjCache.end())
	{
		ObjCache.insert(std::make_pair(key, val));
		return true;
	}
	else
		return false;
}
std::string Server::processRequest(std::string request)
{
	bool procresult;
	std::string replyMsg;
	std::vector<std::string> parts = splitString(request, "$#^");
	if (parts[0] == "addint") {
		if (std::stoi(parts[2]) > INT_MAX || std::stoi(parts[2]) < INT_MIN)
			return "Out of Bounds Error!\n";
		procresult = addInt(std::stoi(parts[1]), std::stoi(parts[2]));
		return procresult ? "addedInt" : "error , key already exists";
	}
	else if (parts[0] == "addstr") {
		procresult = addStr(std::stoi(parts[1]), parts[2]);
		return procresult ? "addedStr" : "error ,key already exists";
	}
	else if (parts[0] == "addobj")
	{
		Customclass obj(std::stoi(parts[2]), parts[3]);
		procresult = addObj(std::stoi(parts[1]), obj);
		return procresult ? "addedObject" : "error,Key already exists";
	}
	else if (parts[0] == "getint")
	{
		int key = std::stoi(parts[1]);
		if (IntCache.find(key) == IntCache.end())
			return "Key " + std::to_string(key) + " does not exist";
		return "Int$#^" + std::to_string(IntCache.at(key));
	}
	else if (parts[0] == "getstr")
	{
		int key = std::stoi(parts[1]);
		if (StrCache.find(key) == StrCache.end())
			return "Key " + std::to_string(key) + " does not exist";
		return "Str$#^" + StrCache.at(key);
	}
	else if (parts[0] == "getobj")
	{
		int key = std::stoi(parts[1]);
		if (ObjCache.find(key) == ObjCache.end())
			return "Key " + std::to_string(key) + " does not exist";
		Customclass obj = ObjCache.at(key);
		std::string attr;
		attr.append("ObjAttr$#^");
		attr.append(std::to_string(obj.getuid()));
		attr.append("$#^" + obj.getname());
		return attr;
	}
	else if (parts[0] == "callfunction")
	{
		int key = std::stoi(parts[1]);
		if (ObjCache.find(key) == ObjCache.end())
			return "Key " + std::to_string(key) + " does not exist";
		Customclass obj = ObjCache.at(key);
		if (parts[2] == "getuid")
			return "FnOut$#^" + std::to_string(obj.getuid());
		else if (parts[2] == "getname")
			return "FnOut$#^" + obj.getname();
	}
	else
		return "Wrong statement\n Usage : [Command]$#^[key]$#^[attributes]\n";
	return "Wrong statement\n Usage : [Command]$#^[key]$#^[attributes]\n";

}
int Server::startServ()
{
	//LPCTSTR lpszPipeName = TEXT("\\\\.\\pipe\\ServerPipe");
	hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	if (hEvent == NULL)
	{
		std::cout << "Create Event failed\n";
		return false;
	}
	else
	{
		std::cout << "Event Created\n";
	}
	Pipe.oOverlap.hEvent = hEvent;
	Pipe.hPipeInst = CreateNamedPipe(TEXT(lpszPipename),
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		1, 4096 * sizeof(TCHAR), 4096 * sizeof(TCHAR), 5000, NULL);
	if (Pipe.hPipeInst == INVALID_HANDLE_VALUE)
	{
		std::cout << "Create NamedPipe failed\n";
		return 0;
	}
	Pipe.fPendingIO = ConnectToNewClient(Pipe.hPipeInst, &Pipe.oOverlap);
	Pipe.dwState = Pipe.fPendingIO ? CONNECTING_STATE : READING_STATE;

	return 1;

}
int Server::run()
{
	BOOL fsuccess;
	DWORD dwait, cbret, dwerr;

	while (1)
	{
		dwait = WaitForSingleObject(hEvent, 200000);
		if (Pipe.fPendingIO)
		{
			fsuccess = GetOverlappedResult(Pipe.hPipeInst, &Pipe.oOverlap, &cbret, FALSE);

			switch (Pipe.dwState)
			{
			case CONNECTING_STATE:
				if (!fsuccess)
				{
					std::cout << "Connecting :" << GetLastError();
					return 0;
					//	continue;
				}
				else
				{
					std::cout << "Client has connected";
				}
				Pipe.dwState = READING_STATE;
				break;
			case READING_STATE:
				if (!fsuccess || cbret == 0)
				{
					std::cout << "\n\nfsuccess is false and cbret = 0 ,Client disconnected or something went wrong while reading\n";
					DisconnectnReconnect();
					continue;
				}
				Pipe.cbRead = cbret;
				Pipe.dwState = WRITING_STATE;
				break;
			case WRITING_STATE:
				if (!fsuccess || cbret != Pipe.cbToWrite)
				{
					std::cout << "Something went wrong while writing \n";
					DisconnectnReconnect();
					continue;
				}
				Pipe.dwState = READING_STATE;
				break;
			default:
			{
				std::cout << "Invalid state\n";
				return 0;

			}
			}
		}
		switch (Pipe.dwState)
		{
		case READING_STATE:
			fsuccess = ReadFile(Pipe.hPipeInst, Pipe.chRequest, 4096 * sizeof(TCHAR), &Pipe.cbRead, &Pipe.oOverlap);
			if (fsuccess && Pipe.cbRead != 0)
			{
				Pipe.fPendingIO = FALSE;
				Pipe.dwState = WRITING_STATE;
				std::cout << "Read Operation Completed\n";
				continue;
			}
			dwerr = GetLastError();
			if (!fsuccess && dwerr == ERROR_IO_PENDING)
			{
				Pipe.fPendingIO = TRUE;
				continue;
			}
			std::cout << "Something went wrong while reading , disconnecting and ready to reconnect\n";
			DisconnectnReconnect();
			break;
		case WRITING_STATE:
			GetAnswerToRequest(&Pipe);

			std::cout << "Snding msg : " << Pipe.chReply;
			fsuccess = WriteFile(Pipe.hPipeInst, Pipe.chReply, Pipe.cbToWrite, &cbret, &Pipe.oOverlap);
			if (fsuccess && cbret == Pipe.cbToWrite)
			{
				Pipe.fPendingIO = FALSE;
				Pipe.dwState = READING_STATE;
				continue;
			}
			dwerr = GetLastError();
			if (!fsuccess && dwerr == ERROR_IO_PENDING)
			{
				Pipe.fPendingIO = TRUE;
				continue;
			}

			std::cout << "Something went wrong , Disconnect and Reconnect\n";
			DisconnectnReconnect();
			break;
		default:
		{
			std::cout << "Invalid pipe state\n";
			return 0;
		}


		}
	}
	return 1;
}
BOOL Server::ConnectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo)
{
	BOOL fConnected, fPendingIO = FALSE;

	fConnected = ConnectNamedPipe(hPipe, lpo);

	if (fConnected)
	{
		printf("ConnectNamedPipe failed with %d.\n", GetLastError());
		return 0;
	}

	switch (GetLastError())
	{
	case ERROR_IO_PENDING:
		fPendingIO = TRUE;
		break;


	case ERROR_PIPE_CONNECTED:
		if (SetEvent(lpo->hEvent))
			break;

	default:
	{
		printf("ConnectNamedPipe failed with %d.\n", GetLastError());
		return 0;
	}
	}

	return fPendingIO;
}
VOID Server::DisconnectnReconnect()
{
	if (!DisconnectNamedPipe(Pipe.hPipeInst))
	{
		std::cout << "Disconnection failed \n";
	}
	else
	{
		std::cout << "Client Disconnected\n";
	}
	Pipe.fPendingIO = ConnectToNewClient(Pipe.hPipeInst, &Pipe.oOverlap);
	Pipe.dwState = Pipe.fPendingIO ? CONNECTING_STATE : READING_STATE;
}