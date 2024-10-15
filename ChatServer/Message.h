#pragma once
#define _CRT_SECURE_NO_WARNINGS   
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "User.h"
#include <iostream>
#include<unordered_map>
#include <fstream>
#include <string>
#include <vector>
#include <WS2tcpip.h>
#include <sstream>


class Message
{
public:
	enum Command
	{
		NoCommand,
		Loggingin,
		Loggingout,
		Signup,
		Help,
		Sending,
		List,
		Log,
		FalseCommand
	}commands;
	enum Process
	{
		Success,
		Error,
		Shutdown,
		Disconnect
	}result;

	char command = '/';
	int serverCap;

	Command checkCommand(char* data);
	Process Register(SOCKET sock, char* data);
	Process Login(SOCKET sock, char* data);
	Process Logout(SOCKET sock, fd_set& masterSet);
	Process Send(SOCKET sock, char* data);
	Process GetUsers(SOCKET sock);
	Process CommandList(SOCKET sock);
	Process GetLog(SOCKET sock);

	Process recvTcpData(SOCKET sock, char* data, int size);
	Process sendTcpData(SOCKET sock, char* data, int size);

	bool checkRegister(SOCKET sock);

	std::vector<User> curUsers;
private:
	std::unordered_map<std::string, User> allUsers;
};

