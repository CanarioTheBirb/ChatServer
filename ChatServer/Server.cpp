#include "Server.h"

Message::Process Server::Init(int port)
{
	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server == INVALID_SOCKET)
	{
		return Message::Shutdown;
	}
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.S_un.S_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (address.sin_addr.S_un.S_addr == -1)
	{
		return Message::Shutdown;
	}

	int result = bind(server, (sockaddr*)&address, sizeof(address));
	if (result == SOCKET_ERROR)
	{
		return Message::Shutdown;
	}

	std::cout << "The server is listening...\n";
	result = listen(server,serverCap);
	if (result == SOCKET_ERROR)
	{
		return Message::Shutdown;
	}

	return Message::Success;
}
void Server::Broadcast(int port)
{
	while (true)
	{
		int time = 1;
		SOCKET broadcast = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		int result = setsockopt(broadcast, SOL_SOCKET, SO_BROADCAST, (char*)&time, sizeof(time));

		sockaddr_in broadaddr;
		broadaddr.sin_family = AF_INET;
		broadaddr.sin_addr.S_un.S_addr = INADDR_BROADCAST;
		broadaddr.sin_port = htons(port);

		uint8_t size_s = 255;
		char welcome[255];
		memset(welcome, 0, 255);
		strcpy(welcome, "Message thingy");
		
		printf("\r\n[Broadcast Message]: %s", welcome);

		result = sendto(broadcast, welcome, size_s, 0, (sockaddr*)&broadaddr, sizeof(broadaddr));

		std::this_thread::sleep_for(std::chrono::seconds(5));

	}
}
Message::Process Server::ReceiveData(SOCKET sock, char* data, int size, Message& msgH)
{
	msgH.result = msgH.recvTcpData(sock, (char*)&size, 1);
	if (msgH.result == Message::Error)
	{
		int error = WSAGetLastError();

		if (error == 10054 || error == 10004)
		{
			return Message::Disconnect;
		}
		else
		{
			return Message::Shutdown;
		}
	}
	msgH.recvTcpData(sock, data, size);

	return Message::Success;
}
Message::Process Server::SendData(SOCKET sock, char* data, int size, Message& msgH)
{
	msgH.result = msgH.sendTcpData(sock, (char*)&size, 1);
	if (msgH.result == Message::Error)
	{
		int error = WSAGetLastError();

		if (error == 10054 || error == 10004)
		{
			return Message::Disconnect;
		}
		else
		{
			return Message::Shutdown;
		}
	}
	msgH.sendTcpData(sock, data, size);

	return Message::Success;
}
Message::Process Server::Relay(SOCKET sock, char* data, Message& msgH)
{
	for (int i = 0; i < msgH.curUsers.size(); i++)
	{
		if (msgH.curUsers[i].sock != sock && msgH.curUsers[i].sock != server)
		{
			uint8_t size_s = 255;
			char message[255];
			memset(message, 0, 255);
			std::ostringstream msg;
			for (auto j = msgH.curUsers.begin(); j != msgH.curUsers.end(); j++)
			{
				if (j->sock == sock)
				{
					msg << j->username << ": " << data;
				}
			}
			strcpy(message, msg.str().c_str());
			msgH.result = SendData(msgH.curUsers[i].sock, message, size_s, msgH);
			msgH.curUsers[i].chatLog += "\r\n";
			msgH.curUsers[i].chatLog += message;
			if (msgH.result == Message::Disconnect)
			{
				return Message::Disconnect;
			}
			else if (msgH.result == Message::Shutdown)
			{
				return Message::Shutdown;
			}
		}
	}
	return Message::Success;
}
Message::Process Server::AddClient(Message& msgH)
{
	SOCKET client = accept(server, NULL, NULL);
	if (client == INVALID_SOCKET)
	{
		return Message::Shutdown;
	}
	std::cout << "\r\nConnected with a client!";
	FD_SET(client, &sockets);

	uint8_t size_s = 255;
	char welcome[255];
	memset(welcome, 0, 255);
	std::ostringstream msg;
	msg << "Welcome to the Chat Room! Use the '" << msgH.command << "help' command to help navigate the chat room.";
	strcpy(welcome, msg.str().c_str());
	msgH.result = SendData(client, welcome, size_s, msgH);
	if (msgH.result == Message::Disconnect)
	{
		return Message::Disconnect;
	}
	else if (msgH.result == Message::Shutdown)
	{
		return Message::Shutdown;
	}

	return Message::Success;
}
void Server::Stop()
{
	for (int i = 0; i < sockets.fd_count; i++)
	{
		shutdown(sockets.fd_array[i], SD_BOTH);
		closesocket(sockets.fd_array[i]);
	}
}