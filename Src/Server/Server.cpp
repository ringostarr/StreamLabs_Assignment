// NamedPipesCpp.cpp : Defines the entry point for the application.
//


#include <iostream>
#include <windows.h>
#include "../CustomClass/CustomClass.h"
#include "Server.h"
using namespace std;

int main(int argc, const char **argv)
{
	Server serv;
	try
	{
		serv.startServ();
		serv.run();
	}
	catch (std::exception &ex)
	{
		std::cout << ex.what() << endl;
	}
	return 0;
}