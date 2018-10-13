#include<iostream>
#include<string>
#include<vector>
std::vector<std::string> splitString(std::string msg, std::string delim)
{
	std::vector<std::string> tokens;
	//std::string delim = ",";
	size_t prev = 0, pos = 0;
	do
	{
		pos = msg.find(delim, prev);
		if (pos == std::string::npos) pos = msg.length();
		std::string token = msg.substr(prev, pos - prev);
		if (!token.empty()) tokens.push_back(token);
		prev = pos + delim.length();
	} while (pos < msg.length() && prev < msg.length());
	return tokens;


}