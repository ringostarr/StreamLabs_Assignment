#pragma once
#include <iostream>
#include <string>

class Customclass
{
	int uid;
	std::string name;
public:
	Customclass()
	{
		uid = -1;
		name = "";
	}
	Customclass(int _id, std::string _name)
	{
		uid = _id;
		name = _name;
	}
	int getuid()
	{

		return uid;
	}
	std::string getname()
	{

		return name;
	}

};