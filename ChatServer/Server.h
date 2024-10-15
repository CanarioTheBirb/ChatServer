#pragma once
#include "Message.h"
#include <thread>
class Server
{
public:
	SOCKET server;
	int serverCap;

	fd_set sockets;

	Message::Process Init(int port);
	void Broadcast(int port);
	Message::Process ReceiveData(SOCKET sock, char* data, int size, Message& msgH);
	Message::Process SendData(SOCKET sock, char* data, int size, Message& msgH);
	Message::Process Relay(SOCKET sock, char* data, Message& msgH);
	Message::Process AddClient(Message& msgH);
	void Stop();
};

