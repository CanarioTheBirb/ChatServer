#include "Message.h"


Message::Command Message::checkCommand(char* data)
{
	std::string message(data);
	if (message[0] != command)
	{
		return NoCommand;
	}
	else
	{
		if (message.find("login") != std::string::npos)
		{
			return Loggingin;
		}
		if (message.find("register") != std::string::npos)
		{
			return Signup;
		}
		if (message.find("help") != std::string::npos)
		{
			return Help;
		}
		if (message.find("send") != std::string::npos)
		{
			return Sending;
		}
		if (message.find("list") != std::string::npos)
		{
			return List;
		}
		if (message.find("logout") != std::string::npos)
		{
			return Loggingout;
		}
		if (message.find("log") != std::string::npos)
		{
			return Log;
		}
	}
	return FalseCommand;
}

Message::Process Message::Register(SOCKET sock, char* data)
{
	std::stringstream split(data);
	std::string user;

	User newUser;
	while (std::getline(split, user, ' '))
	{
		if (user.find("register") == std::string::npos && newUser.username == "")
		{
			newUser.username = user;
		}
		else if (user.find("register") == std::string::npos && newUser.password == "")
		{
			newUser.password = user;
		}
	}

	if (newUser.username == "" || newUser.password == "")
	{
		uint8_t size = 255;
		char message[255];
		memset(message, 0, 255);
		strcpy(message, "Username or Password information was not filled in correctly try again");
		result = sendTcpData(sock, (char*)&size, 1);
		if (result == Message::Error)
		{
			int error = WSAGetLastError();

			if (error == 10054 || error == 10004)
			{
				return Disconnect;
			}
			else
			{
				return Shutdown;
			}
		}
		sendTcpData(sock, message, size);
		printf("\r\n[Received]: %s", data);
		curUsers[0].chatLog += "\r\n[Received]: ";
		curUsers[0].chatLog += data;
		printf("\r\n[User Creation Failed]: Username or Password not information missing or incorrect format");
		curUsers[0].chatLog += "\r\n[User Creation Failed]: Username or Password not information missing or incorrect format";
	}
	else if (allUsers.size() == serverCap)
	{
		uint8_t size = 255;
		char message[255];
		memset(message, 0, 255);
		strcpy(message, "Register amount reached");
		result = sendTcpData(sock, (char*)&size, 1);
		if ((result == SOCKET_ERROR) || (result == Message::Error))
		{
			int error = WSAGetLastError();

			if (error == 10054 || error == 10004)
			{
				return Disconnect;
			}
			else
			{
				return Shutdown;
			}
		}
		sendTcpData(sock, message, size);
		for (auto j = curUsers.begin(); j != curUsers.end(); j++)
		{
			if (sock == j->sock)
			{
				j->chatLog += "\r\n";
				j->chatLog += message;
			}
		}
		printf("\r\n[Received]: %s", data);
		curUsers[0].chatLog += "\r\n[Received]: ";
		curUsers[0].chatLog += data;
		printf("\r\n[User Creation Failed]: Max Registration reached");
		curUsers[0].chatLog += "\r\n[User Creation Failed]: Max Registration reached";
	}
	else if (allUsers.find(newUser.username) != allUsers.end())
	{
		uint8_t size = 255;
		char message[255];
		memset(message, 0, 255);
		strcpy(message, "Username already in use, try another username.");
		result = sendTcpData(sock, (char*)&size, 1);
		if ((result == SOCKET_ERROR) || (result == Message::Error))
		{
			int error = WSAGetLastError();

			if (error == 10054 || error == 10004)
			{
				return Disconnect;
			}
			else
			{
				return Shutdown;
			}
		}
		sendTcpData(sock, message, size);
		printf("\r\n[Received]: %s", data);
		curUsers[0].chatLog += "\r\n[Received]: ";
		curUsers[0].chatLog += data;
		printf("\r\n[User Creation Failed]: Username already in use.");
		curUsers[0].chatLog += "\r\n[User Creation Failed]: Username already in use.";
	}
	else
	{
		uint8_t size = 255;
		char message[255];
		memset(message, 0, 255);
		strcpy(message, "User made successfully");
		result = sendTcpData(sock, (char*)&size, 1);
		if (result == Message::Error)
		{
			int error = WSAGetLastError();

			if (error == 10054 || error == 10004)
			{
				return Disconnect;
			}
			else
			{
				return Shutdown;
			}
		}
		sendTcpData(sock, message, size);
		allUsers[newUser.username] = newUser;
		printf("\r\n[Received]: %s", data);
		curUsers[0].chatLog += "\r\n[Received]: ";
		curUsers[0].chatLog += data;
		std::cout << "\r\n[User Created]: " << newUser.username;
		curUsers[0].chatLog += "\r\n[User Created]: " + newUser.username;
	}
	return Success;
}
Message::Process Message::Login(SOCKET sock, char* data)
{
	std::stringstream split(data);
	std::string user;

	User newUser;
	while (std::getline(split, user, ' '))
	{
		if (user.find("login") == std::string::npos && newUser.username == "")
		{
			newUser.username = user;
		}
		else if (user.find("login") == std::string::npos && newUser.password == "")
		{
			newUser.password = user;
		}
	}

	if (newUser.username == "" || newUser.password == "")
	{
		uint8_t size = 255;
		char message[255];
		memset(message, 0, 255);
		strcpy(message, "Username or Password information was not filled in correctly try again");
		result = sendTcpData(sock, (char*)&size, 1);
		if (result == Message::Error)
		{
			int error = WSAGetLastError();

			if (error == 10054 || error == 10004)
			{
				return Disconnect;
			}
			else
			{
				return Shutdown;
			}
		}
		sendTcpData(sock, message, size);
		printf("\r\n[Received]: %s", data);
		curUsers[0].chatLog += "\r\n[Received]: ";
		curUsers[0].chatLog += data;
		printf("\r\n[User Login Failed]: Username or Password not information missing or incorrect format");
		curUsers[0].chatLog += "\r\n[User Login Failed]: Username or Password not information missing or incorrect format";
	}
	else if (allUsers.find(newUser.username) == allUsers.end())
	{
		uint8_t size = 255;
		char message[255];
		memset(message, 0, 255);
		strcpy(message, "User not found, try again.");
		result = sendTcpData(sock, (char*)&size, 1);
		if ((result == SOCKET_ERROR) || (result == Message::Error))
		{
			int error = WSAGetLastError();

			if (error == 10054 || error == 10004)
			{
				return Disconnect;
			}
			else
			{
				return Shutdown;
			}
		}
		sendTcpData(sock, message, size);
		printf("\r\n[Received]: %s", data);
		curUsers[0].chatLog += "\r\n[Received]: ";
		curUsers[0].chatLog += data;
		printf("\r\n[User Login Failed]: Wrong Username.");
		curUsers[0].chatLog += "\r\n[User Login Failed]: Wrong Username.";
	}
	else if (allUsers.find(newUser.username) != allUsers.end() && allUsers[newUser.username].password != newUser.password)
	{
		uint8_t size = 255;
		char message[255];
		memset(message, 0, 255);
		strcpy(message, "Password is wrong, try again.");
		result = sendTcpData(sock, (char*)&size, 1);
		if ((result == SOCKET_ERROR) || (result == Message::Error))
		{
			int error = WSAGetLastError();

			if (error == 10054 || error == 10004)
			{
				return Disconnect;
			}
			else
			{
				return Shutdown;
			}
		}
		sendTcpData(sock, message, size);
		printf("\r\n[Received]: %s", data);
		curUsers[0].chatLog += "\r\n[Received]: ";
		curUsers[0].chatLog += data;
		printf("\r\n[User Login Failed]: Wrong Password.");
		curUsers[0].chatLog += "\r\n[User Login Failed]: Wrong Password.";
	}
	else
	{
		for (auto i = curUsers.begin(); i != curUsers.end(); i++)
		{
			if (i->username == newUser.username)
			{
				uint8_t size = 255;
				char message[255];
				memset(message, 0, 255);
				strcpy(message, "User already logged in");
				result = sendTcpData(sock, (char*)&size, 1);
				if (result == Message::Error)
				{
					int error = WSAGetLastError();

					if (error == 10054 || error == 10004)
					{
						return Disconnect;
					}
					else
					{
						return Shutdown;
					}
				}
				sendTcpData(sock, message, size);
				printf("\r\n[Received]: %s", data);
				curUsers[0].chatLog += "\r\n[Received]: ";
				curUsers[0].chatLog += data;
				printf("[Login Error]: User already logged in.");
				curUsers[0].chatLog += "[Login Error]: User already logged in.";

				return Success;
			}
		}
		uint8_t size = 255;
		char message[255];
		memset(message, 0, 255);
		strcpy(message, "Login Successful");
		result = sendTcpData(sock, (char*)&size, 1);
		if (result == Message::Error)
		{
			int error = WSAGetLastError();

			if (error == 10054 || error == 10004)
			{
				return Disconnect;
			}
			else
			{
				return Shutdown;
			}
		}
		sendTcpData(sock, message, size);
		printf("\r\n[Received]: %s", data);
		newUser.sock = sock;
		curUsers.push_back(newUser);
		curUsers[0].chatLog += "\r\n[Received]: ";
		curUsers[0].chatLog += data;
		std::cout << "\r\n[Logged In]: " << newUser.username;
		curUsers[0].chatLog += "\r\n[Logged In]: " + newUser.username;
	}
	return Success;
}
Message::Process Message::Logout(SOCKET sock, fd_set& masterSet)
{
	FD_CLR(sock, &masterSet);
	closesocket(sock);
	for (auto i = curUsers.begin(); i != curUsers.end(); i++)
	{
		if ((*i).sock == sock)
		{
			curUsers.erase(i);
			break;
		}
	}
	printf("\r\n[Client Error]: A Client Disconnected");
	curUsers[0].chatLog += "\r\n[Client Error]: A Client Disconnected";
	return Success;
}
Message::Process Message::Send(SOCKET sock, char* data)
{
	std::stringstream split(data);
	std::string user;
	std::ostringstream msg;

	for (auto j = curUsers.begin(); j != curUsers.end(); j++)
	{
		if ((*j).sock == sock)
		{
			msg << (*j).username << ":";
		}
	}

	User newUser;
	while (std::getline(split, user, ' '))
	{
		if (user.find("send") == std::string::npos && newUser.username == "")
		{
			newUser.username = user;
		}
		else if ((user.find("send") == std::string::npos))
		{
			msg << " " << user;
		}
	}
	uint8_t size = 255;
	char message[255];
	memset(message, 0, 255);
	strcpy(message, msg.str().c_str());

	for (auto i = curUsers.begin(); i != curUsers.end(); i++)
	{
		if (i->username == newUser.username)
		{
			result = sendTcpData(i->sock, (char*)&size, 1);
			if ((result == SOCKET_ERROR) || (result == Message::Error))
			{
				int error = WSAGetLastError();

				if (error == 10054 || error == 10004)
				{
					return Disconnect;
				}
				else
				{
					return Shutdown;
				}
			}
			sendTcpData(i->sock, message, size);
			break;
		}
	}
	return Success;
}
Message::Process Message::GetUsers(SOCKET sock)
{
	uint8_t size = 255;
	char message[255];
	memset(message, 0, 255);
	std::ostringstream msg;
	msg << "Current logged in Users:\n";
	for (auto i = curUsers.begin(); i != curUsers.end(); i++)
	{
		if (i->username != "" && i->username != "Server")
		{
			msg << "-" << i->username << "\n";
		}
	}
	strcpy(message, msg.str().c_str());
	result = sendTcpData(sock, (char*)&size, 1);
	if ((result == SOCKET_ERROR) || (result == Message::Error))
	{
		int error = WSAGetLastError();

		if (error == 10054 || error == 10004)
		{
			return Disconnect;
		}
		else
		{
			return Shutdown;
		}
	}
	sendTcpData(sock, message, size);
	for (auto j = curUsers.begin(); j != curUsers.end(); j++)
	{
		if (sock == j->sock)
		{
			j->chatLog += "\r\n";
			j->chatLog += message;
		}
	}
	printf("\r\n[Command request]: Get active users.");
	curUsers[0].chatLog += "\r\n[Command request]: Get active users.";
	return Success;
}
Message::Process Message::CommandList(SOCKET sock)
{
	uint8_t size = 255;
	char message[255];
	memset(message, 0, 255);
	std::ostringstream msg;
	msg << "The commands are: " << command << "register <username> <password>, " << command << "login <username> <password>," << command << "send <user> <message>,"
		<< command << "list, " << command << "logout, " << command << "log.";
	strcpy(message, msg.str().c_str());
	result = sendTcpData(sock, (char*)&size, 1);
	if (result == Message::Error)
	{
		int error = WSAGetLastError();

		if (error == 10054 || error == 10004)
		{
			return Disconnect;
		}
		else
		{
			return Shutdown;
		}
	}
	sendTcpData(sock, message, size);

	for (auto j = curUsers.begin(); j != curUsers.end(); j++)
	{
		if (sock == j->sock)
		{
			j->chatLog += "\r\n";
			j->chatLog += message;
		}
	}
	return Success;
}
Message::Process Message::recvTcpData(SOCKET sock, char* data, int size)
{
	int total = 0;

	do
	{
		int ret = recv(sock, data + total, size - total, 0);
		if (ret < 1)
			return Error;
		else
			total += ret;

	} while (total < size);

	return Success;
}

Message::Process Message::sendTcpData(SOCKET sock, char* data, int size)
{
	int result;
	int bytesSent = 0;

	while (bytesSent < size)
	{
		result = send(sock, (const char*)data + bytesSent, size - bytesSent, 0);

		if (result <= 0)
			return Error;

		bytesSent += result;
	}

	return Success;
}

bool Message::checkRegister(SOCKET sock)
{
	for (auto i = curUsers.begin(); i != curUsers.end(); i++)
	{
		if (i->sock == sock)
		{
			return true;
		}
	}
	return false;
}

Message::Process Message::GetLog(SOCKET sock)
{
	for (auto i = curUsers.begin(); i != curUsers.end(); i++)
	{
		if (i->sock == sock)
		{
			std::ofstream logs(i->username + ".txt");
			if (logs.is_open())
			{
				logs << i->chatLog;
			}
			logs.close();

			uint8_t size = 255;
			char message[255];
			memset(message, 0, 255);
			strcpy(message, "Logged Successfully");
			result = sendTcpData(sock, (char*)&size, 1);
			if (result == Message::Error)
			{
				int error = WSAGetLastError();

				if (error == 10054 || error == 10004)
				{
					return Disconnect;
				}
				else
				{
					return Shutdown;
				}
			}
			sendTcpData(sock, message, size);
			for (auto j = curUsers.begin(); j != curUsers.end(); j++)
			{
				if (sock == j->sock)
				{
					j->chatLog += "\r\n";
					j->chatLog += message;
				}
			}
			printf("\r\n[Received]: %s", message);
			curUsers[0].chatLog += "\r\n[Received]: ";
			curUsers[0].chatLog += message;
			break;
		}
	}
	return Success;
}