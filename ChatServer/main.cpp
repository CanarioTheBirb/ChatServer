#include "Server.h"
using namespace std;

int main()
{
	WSADATA wsadata;
	WSAStartup(WINSOCK_VERSION, &wsadata);
	Message msgH;
	Server server;

	int port = 0;
	printf("Whats the port of the server: ");
	cin >> port;
	cin.clear();
	cin.ignore();
	printf("Whats the server capacity: ");
	cin >> server.serverCap;
	cin.clear();
	cin.ignore();
	printf("Whats the command character (default is '/'): ");
	cin >> msgH.command;
	cin.clear();
	cin.ignore();
	msgH.serverCap = server.serverCap;
	//clear screen for now until find something better
	system("cls");

	int result = 0;
	string name;
	result = gethostname((char*)name.c_str(), sizeof(name));
	if (result == -1)
	{
		printf("\r\n [Server Error]: Unable to continue Shutting Down due to error %i", WSAGetLastError());
		return 0;
	}
	printf("Host Name is: ");
	printf(name.c_str());
	printf("\n");

	addrinfo hints;
	addrinfo* res, * p;
	char ipstr[INET6_ADDRSTRLEN];
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	result = getaddrinfo(name.c_str(), NULL, &hints, &res);
	if (result == -1)
	{
		printf("\r\n [Server Error]: Unable to continue Shutting Down due to error %i", WSAGetLastError());
		return 0;
	}

	for (p = res; p != NULL; p = p->ai_next)
	{
		void* addr;
		char* ipver;

		if (p->ai_family == AF_INET)
		{ // IPv4
			struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = (char*)"IPv4";
		}
		else
		{ // IPv6
			struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = (char*)"IPv6";
		}

		// convert the IP to a string and print it:
		inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
		printf("  %s: %s\n", ipver, ipstr);
	}

	freeaddrinfo(res);

	msgH.result = server.Init(port);

	if (msgH.result == Message::Shutdown)
	{
		printf("\r\n [Server Error]: Unable to continue Shutting Down due to error %i", WSAGetLastError());
		return 0;
	}
	else
	{
		fd_set tempSet;
		timeval timer;

		FD_ZERO(&server.sockets);
		FD_SET(server.server, &server.sockets);

		msgH.curUsers.push_back(User("Server", "admin", server.server));

		timer.tv_sec = 0;
		timer.tv_usec = 100000;

		uint8_t size = 255;
		char* buffer = new char[size];

		thread udpSend(&Server::Broadcast,server,port);

		while (true)
		{
			tempSet = server.sockets;

			int readFd = select(tempSet.fd_count, &tempSet, NULL, NULL, &timer);
			if (readFd == SOCKET_ERROR)
			{
				printf("\r\n [Server Error]: Unable to continue Shutting Down due to error %i", WSAGetLastError());
				msgH.curUsers[0].chatLog += "\r\n [Server Error]: Unable to continue Shutting Down due to error " + WSAGetLastError();
				delete buffer;
				break;
			}

			for (int i = 0; i < readFd; i++)
			{
				if (tempSet.fd_array[i] == server.server)
				{
					if (FD_ISSET(server.server, &tempSet))
					{
						msgH.result = server.AddClient(msgH);
						msgH.curUsers[0].chatLog += "\r\nConnected with a client!";
						if (msgH.result == Message::Disconnect)
						{
							printf("\r\n[Client Error]: A Client Disconnected");
							msgH.curUsers[0].chatLog += "\r\n[Client Error]: A Client Disconnected";

						}
						else if (msgH.result == Message::Shutdown)
						{
							printf("\r\n[Server Error]: Unable to continue Shutting Down due to error %i", WSAGetLastError());
							msgH.curUsers[0].chatLog += "\r\n [Server Error]: Unable to continue Shutting Down due to error " + WSAGetLastError();
							delete buffer;
							break;
						}
					}
				}
				else
				{
					if (FD_ISSET(tempSet.fd_array[i], &tempSet))
					{
						msgH.result = server.ReceiveData(tempSet.fd_array[i], buffer, size, msgH);
						for (auto j = msgH.curUsers.begin(); j != msgH.curUsers.end(); j++)
						{
							if (tempSet.fd_array[i] == j->sock)
							{
								j->chatLog += "\r\n";
								j->chatLog += buffer;
							}
						}
						if (msgH.result == Message::Disconnect)
						{
							printf("\r\n[Client Error]: A Client Disconnected");
							msgH.curUsers[0].chatLog += "\r\n[Client Error]: A Client Disconnected";
							FD_CLR(tempSet.fd_array[i], &server.sockets);
							closesocket(tempSet.fd_array[i]);
							for (auto j = msgH.curUsers.begin(); j != msgH.curUsers.end(); j++)
							{
								if (tempSet.fd_array[i] == j->sock)
								{
									msgH.curUsers.erase(j);
									break;
								}
							}
						}
						else if (msgH.result == Message::Shutdown)
						{
							printf("\r\n[Server Error]: Unable to continue Shutting Down due to error %i", WSAGetLastError());
							msgH.curUsers[0].chatLog += "\r\n [Server Error]: Unable to continue Shutting Down due to error " + WSAGetLastError();
							delete buffer;
							break;
						}

						if (msgH.result != Message::Disconnect)
						{
							msgH.commands = msgH.checkCommand(buffer);
							if (msgH.commands == Message::NoCommand)
							{
								if (msgH.checkRegister(tempSet.fd_array[i]))
								{
									for (auto j = msgH.curUsers.begin(); j != msgH.curUsers.end(); j++)
									{
										if (tempSet.fd_array[i] == j->sock)
										{
											std::cout << "\r\n[" << j->username << "]: " << buffer;
											msgH.curUsers[0].chatLog += "\r\n[" + j->username + "]: " + buffer;
											break;
										}
									}
									msgH.result = server.Relay(tempSet.fd_array[i], buffer, msgH);
									if (msgH.result == Message::Disconnect)
									{
										printf("\r\n[Client Error]: A Client Disconnected");
										msgH.curUsers[0].chatLog += "\r\n[Client Error]: A Client Disconnected";
										FD_CLR(tempSet.fd_array[i], &server.sockets);
										closesocket(tempSet.fd_array[i]);
										for (auto j = msgH.curUsers.begin(); j != msgH.curUsers.end(); j++)
										{
											if (tempSet.fd_array[i] == j->sock)
											{
												msgH.curUsers.erase(j);
												break;
											}
										}
									}
									else if (msgH.result == Message::Shutdown)
									{
										printf("\r\n[Server Error]: Unable to continue Shutting Down due to error %i", WSAGetLastError());
										msgH.curUsers[0].chatLog += "\r\n [Server Error]: Unable to continue Shutting Down due to error " + WSAGetLastError();
										delete buffer;
										break;
									}
								}
								else
								{
									uint8_t size_e = 255;
									char error[255];
									memset(error, 0, 255);
									std::ostringstream msg;
									msg << "Please register then log in first before messaging";
									strcpy(error, msg.str().c_str());
									msgH.result = server.SendData(tempSet.fd_array[i], error, size_e, msgH);
									if (msgH.result == Message::Disconnect)
									{
										printf("\r\n[Client Error]: A Client Disconnected");
										msgH.curUsers[0].chatLog += "\r\n[Client Error]: A Client Disconnected";
										FD_CLR(tempSet.fd_array[i], &server.sockets);
										closesocket(tempSet.fd_array[i]);
										for (auto j = msgH.curUsers.begin(); j != msgH.curUsers.end(); j++)
										{
											if (tempSet.fd_array[i] == j->sock)
											{
												msgH.curUsers.erase(j);
												break;
											}
										}
									}
									else if (msgH.result == Message::Shutdown)
									{
										printf("\r\n[Server Error]: Unable to continue Shutting Down due to error %i", WSAGetLastError());
										msgH.curUsers[0].chatLog += "\r\n [Server Error]: Unable to continue Shutting Down due to error " + WSAGetLastError();
										delete buffer;
										break;
									}
								}
							}
							else if (msgH.commands == Message::Help)
							{
								msgH.result = msgH.CommandList(tempSet.fd_array[i]);
								if (msgH.result == Message::Disconnect)
								{
									printf("\r\n[Client Error]: A Client Disconnected");
									msgH.curUsers[0].chatLog += "\r\n[Client Error]: A Client Disconnected";
									FD_CLR(tempSet.fd_array[i], &server.sockets);
									closesocket(tempSet.fd_array[i]);
									for (auto j = msgH.curUsers.begin(); j != msgH.curUsers.end(); j++)
									{
										if (tempSet.fd_array[i] == j->sock)
										{
											msgH.curUsers.erase(j);
											break;
										}
									}
								}
								else if (msgH.result == Message::Shutdown)
								{
									printf("\r\n[Server Error]: Unable to continue Shutting Down due to error %i", WSAGetLastError());
									msgH.curUsers[0].chatLog += "\r\n [Server Error]: Unable to continue Shutting Down due to error " + WSAGetLastError();
									delete buffer;
									break;
								}
							}
							else if (msgH.commands == Message::Signup)
							{
								msgH.result = msgH.Register(tempSet.fd_array[i], buffer);
								if (msgH.result == Message::Disconnect)
								{
									printf("\r\n[Client Error]: A Client Disconnected");
									msgH.curUsers[0].chatLog += "\r\n[Client Error]: A Client Disconnected";
									FD_CLR(tempSet.fd_array[i], &server.sockets);
									closesocket(tempSet.fd_array[i]);
									for (auto j = msgH.curUsers.begin(); j != msgH.curUsers.end(); j++)
									{
										if (tempSet.fd_array[i] == j->sock)
										{
											msgH.curUsers.erase(j);
											break;
										}
									}
								}
								else if (msgH.result == Message::Shutdown)
								{
									printf("\r\n[Server Error]: Unable to continue Shutting Down due to error %i", WSAGetLastError());
									msgH.curUsers[0].chatLog += "\r\n [Server Error]: Unable to continue Shutting Down due to error " + WSAGetLastError();
									delete buffer;
									break;
								}
							}
							else if (msgH.commands == Message::Loggingin)
							{
								msgH.result = msgH.Login(tempSet.fd_array[i], buffer);
								if (msgH.result == Message::Disconnect)
								{
									printf("\r\n[Client Error]: A Client Disconnected");
									msgH.curUsers[0].chatLog += "\r\n[Client Error]: A Client Disconnected";
									FD_CLR(tempSet.fd_array[i], &server.sockets);
									closesocket(tempSet.fd_array[i]);
									for (auto j = msgH.curUsers.begin(); j != msgH.curUsers.end(); j++)
									{
										if (tempSet.fd_array[i] == j->sock)
										{
											msgH.curUsers.erase(j);
											break;
										}
									}
								}
								else if (msgH.result == Message::Shutdown)
								{
									printf("\r\n[Server Error]: Unable to continue Shutting Down due to error %i", WSAGetLastError());
									msgH.curUsers[0].chatLog += "\r\n [Server Error]: Unable to continue Shutting Down due to error " + WSAGetLastError();
									delete buffer;
									break;
								}
							}
							else if (msgH.commands == Message::Sending)
							{
								if (msgH.checkRegister(tempSet.fd_array[i]))
								{
									msgH.result = msgH.Send(tempSet.fd_array[i], buffer);
									if (msgH.result == Message::Disconnect)
									{
										printf("\r\n[Client Error]: A Client Disconnected");
										msgH.curUsers[0].chatLog += "\r\n[Client Error]: A Client Disconnected";
										FD_CLR(tempSet.fd_array[i], &server.sockets);
										closesocket(tempSet.fd_array[i]);
										for (auto j = msgH.curUsers.begin(); j != msgH.curUsers.end(); j++)
										{
											if (tempSet.fd_array[i] == j->sock)
											{
												msgH.curUsers.erase(j);
												break;
											}
										}
									}
									else if (msgH.result == Message::Shutdown)
									{
										printf("\r\n[Server Error]: Unable to continue Shutting Down due to error %i", WSAGetLastError());
										msgH.curUsers[0].chatLog += "\r\n [Server Error]: Unable to continue Shutting Down due to error " + WSAGetLastError();
										delete buffer;
										break;
									}
								}
								else
								{
									uint8_t size_e = 255;
									char error[255];
									memset(error, 0, 255);
									std::ostringstream msg;
									msg << "Please register then log in first before messaging";
									strcpy(error, msg.str().c_str());
									msgH.result = server.SendData(tempSet.fd_array[i], error, size_e, msgH);
									if (msgH.result == Message::Disconnect)
									{
										printf("\r\n[Client Error]: A Client Disconnected");
										msgH.curUsers[0].chatLog += "\r\n[Client Error]: A Client Disconnected";
										FD_CLR(tempSet.fd_array[i], &server.sockets);
										closesocket(tempSet.fd_array[i]);
										for (auto j = msgH.curUsers.begin(); j != msgH.curUsers.end(); j++)
										{
											if (tempSet.fd_array[i] == j->sock)
											{
												msgH.curUsers.erase(j);
												break;
											}
										}
									}
									else if (msgH.result == Message::Shutdown)
									{
										printf("\r\n[Server Error]: Unable to continue Shutting Down due to error %i", WSAGetLastError());
										msgH.curUsers[0].chatLog += "\r\n [Server Error]: Unable to continue Shutting Down due to error " + WSAGetLastError();
										delete buffer;
										break;
									}
								}
							}
							else if (msgH.commands == Message::Loggingout)
							{
								if (msgH.checkRegister(tempSet.fd_array[i]))
								{
									msgH.Logout(tempSet.fd_array[i], server.sockets);
								}
								else
								{
									uint8_t size_e = 255;
									char error[255];
									memset(error, 0, 255);
									std::ostringstream msg;
									msg << "Please register then log in first before messaging";
									strcpy(error, msg.str().c_str());
									msgH.result = server.SendData(tempSet.fd_array[i], error, size_e, msgH);
									if (msgH.result == Message::Disconnect)
									{
										printf("\r\n[Client Error]: A Client Disconnected");
										msgH.curUsers[0].chatLog += "\r\n[Client Error]: A Client Disconnected";
										FD_CLR(tempSet.fd_array[i], &server.sockets);
										closesocket(tempSet.fd_array[i]);
										for (auto j = msgH.curUsers.begin(); j != msgH.curUsers.end(); j++)
										{
											if (tempSet.fd_array[i] == j->sock)
											{
												msgH.curUsers.erase(j);
												break;
											}
										}
									}
									else if (msgH.result == Message::Shutdown)
									{
										printf("\r\n[Server Error]: Unable to continue Shutting Down due to error %i", WSAGetLastError());
										msgH.curUsers[0].chatLog += "\r\n [Server Error]: Unable to continue Shutting Down due to error " + WSAGetLastError();
										delete buffer;
										break;
									}
								}
							}
							else if (msgH.commands == Message::List)
							{
								msgH.result = msgH.GetUsers(tempSet.fd_array[i]);
								if (msgH.result == Message::Disconnect)
								{
									printf("\r\n[Client Error]: A Client Disconnected");
									msgH.curUsers[0].chatLog += "\r\n[Client Error]: A Client Disconnected";
									FD_CLR(tempSet.fd_array[i], &server.sockets);
									closesocket(tempSet.fd_array[i]);
									for (auto j = msgH.curUsers.begin(); j != msgH.curUsers.end(); j++)
									{
										if (tempSet.fd_array[i] == j->sock)
										{
											msgH.curUsers.erase(j);
											break;
										}
									}
								}
								else if (msgH.result == Message::Shutdown)
								{
									printf("\r\n[Server Error]: Unable to continue Shutting Down due to error %i", WSAGetLastError());
									msgH.curUsers[0].chatLog += "\r\n [Server Error]: Unable to continue Shutting Down due to error " + WSAGetLastError();
									delete buffer;
									break;
								}
							}
							else if (msgH.commands == Message::Log)
							{
								msgH.result = msgH.GetLog(tempSet.fd_array[i]);
								if (msgH.result == Message::Disconnect)
								{
									printf("\r\n[Client Error]: A Client Disconnected");
									msgH.curUsers[0].chatLog += "\r\n[Client Error]: A Client Disconnected";
									FD_CLR(tempSet.fd_array[i], &server.sockets);
									closesocket(tempSet.fd_array[i]);
									for (auto j = msgH.curUsers.begin(); j != msgH.curUsers.end(); j++)
									{
										if (tempSet.fd_array[i] == j->sock)
										{
											msgH.curUsers.erase(j);
											break;
										}
									}
								}
								else if (msgH.result == Message::Shutdown)
								{
									printf("\r\n[Server Error]: Unable to continue Shutting Down due to error %i", WSAGetLastError());
									msgH.curUsers[0].chatLog += "\r\n [Server Error]: Unable to continue Shutting Down due to error " + WSAGetLastError();
									delete buffer;
									break;
								}
							}
							else
							{
								uint8_t size_e = 255;
								char error[255];
								memset(error, 0, 255);
								std::ostringstream msg;
								msg << "The command does not exist use '" << msgH.command << "help' to find the command";
								strcpy(error, msg.str().c_str());
								msgH.result = server.SendData(tempSet.fd_array[i], error, size_e, msgH);
								if (msgH.result == Message::Disconnect)
								{
									printf("\r\n[Client Error]: A Client Disconnected");
									msgH.curUsers[0].chatLog += "\r\n[Client Error]: A Client Disconnected";
									FD_CLR(tempSet.fd_array[i], &server.sockets);
									closesocket(tempSet.fd_array[i]);
									for (auto j = msgH.curUsers.begin(); j != msgH.curUsers.end(); j++)
									{
										if (tempSet.fd_array[i] == j->sock)
										{
											msgH.curUsers.erase(j);
											break;
										}
									}
								}
								else if (msgH.result == Message::Shutdown)
								{
									printf("\r\n[Server Error]: Unable to continue Shutting Down due to error %i", WSAGetLastError());
									msgH.curUsers[0].chatLog += "\r\n [Server Error]: Unable to continue Shutting Down due to error " + WSAGetLastError();
									delete buffer;
									break;
								}

								for (auto j = msgH.curUsers.begin(); j != msgH.curUsers.end(); j++)
								{
									if (tempSet.fd_array[i] == j->sock)
									{
										j->chatLog += "\r\n";
										j->chatLog += error;
										break;
									}
								}
							}

						}

					}
				}
				std::ofstream log("server.txt");
				if (log.is_open())
				{
					log << msgH.curUsers[0].chatLog;
				}
				log.close();
			}
		}
		udpSend.join();
	}
	server.Stop();
	WSACleanup();
	return 0;
}