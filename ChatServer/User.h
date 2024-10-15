#pragma once
#include <iostream>
#include <vector>
#include <winsock2.h>
#pragma comment(lib,"Ws2_32.lib")

struct User
{
	std::string username;
	std::string password;
	SOCKET sock;
	std::string chatLog;
	User()
	{
		username = "";
		password = "";
		sock = -1;
	}
	User(std::string user, std::string pass)
	{
		username = user;
		password = pass;
	}
	User(std::string user, std::string pass, SOCKET socket)
	{
		username = user;
		password = pass;
		sock = socket;
	}
};