/*
* We are going to use select() to create a server capable of handling many client and servers at the same time without using one thread per client paradigme.
* here's the signature:
*    int select(int nfds, fs_set *readfds, fd_set *writefds, fd_set *errorfds, const struct timeval* timeout);
* struct fd_set {
* u_int fd_count;
* SOCKET fd_array[FD_SETSIZE];
* }
* readfds: for receiving sockets
* writesfds: for sending sockets
* errorfds: for problematic sockets
* 
* To work with fd_set array we have a set macros to help us:
* FD_CLR(s, *set) - removes socket s from the fdset set
* FD_ZERO(*set) - intializes the set to null set
* FD_SET(s, *set) - adds socket s to the fdset set
* FD_ISSET(s, *set) -used for checking if socket s is present in fdset set.
* 
* Bellow we are going to create a simple chat server using select, capable of serving multiple clients
*/
#include"TicTacToe.h"
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <array>
#include <string>
#define DEFAULT_PORT 8889

#pragma comment(lib, "Ws2_32.lib") // linking with winsock library
const int max_client{ 3 };
const int MAXRECVSIZE{ 1024 };

struct ClientInfo
{
	char name;
	SOCKET clientSocket =0;
	struct sockaddr_in address;
};

using namespace std;
int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET listenSocket, new_socket, client_socket[max_client];
	array<ClientInfo, max_client> clientArray;
	struct sockaddr_in server, address;
	
	char buffer[MAXRECVSIZE]{ "" };

	fd_set readfds; 

	// initialize client sockets to null
	for (int i = 0; i < max_client; ++i)
	{
		client_socket[i] = 0;
		clientArray[i].clientSocket = 0;
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "Failed to initialize wsa: error=" << WSAGetLastError();
		return 1;
	}

	// create listening socket:
	if ((listenSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		cout << "Could not create ipv4 socket: error=" << WSAGetLastError();
		WSACleanup();
		return 1;
	}

	cout << "Master socket created\n";
	//binding:
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(DEFAULT_PORT);
	if (bind(listenSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		cout << "Bind failed fo rmaster socket: error=" << WSAGetLastError();
		WSACleanup();
		closesocket(listenSocket);
		return 1;
	}
	cout << "Bind done.\n";

	//Listening to incoming connection:
	listen(listenSocket, 10);
	int addrlen{ sizeof(struct sockaddr_in) };
	int Turn = 0;
	int Ready = 0;
	bool doOnce = true;
	TicTacToe ttt;
	bool gameEnd = false;

	cout << "infinite wait loop for incoming connections:\n";
	while (!gameEnd)
	{
		// clear our readfds:
		FD_ZERO(&readfds);
		FD_SET(listenSocket, &readfds);
		for (int i = 0; i < max_client; ++i)
		{
			//SOCKET s = client_socket[i];
			SOCKET s = clientArray[i].clientSocket;
			if (s > 0)
				FD_SET(s, &readfds);
		}


		// wait for an activity on any of the client sockets. Timeout is set to null meaning wait indefinitely
		int activity = select(0, &readfds, NULL, NULL, NULL);
		if (activity == SOCKET_ERROR)
		{
			cout << "Select returned error " << WSAGetLastError();
			WSACleanup();
			// for proper socket clean up, all client sockets and maste socket should be closed here. We ignore it for now.
			return 1;
		}
		// first check master socket for activity:
		if (FD_ISSET(listenSocket, &readfds))
		{
			if ((new_socket = accept(listenSocket, (struct sockaddr*)&address, (int*)&addrlen)) == SOCKET_ERROR)
			{
				cout << "accept failed: error = " << WSAGetLastError();
				WSACleanup();
				return 1;
			}
			char addrString[60];
			InetNtop(address.sin_family, &address.sin_addr, (PWSTR)&addrString, sizeof(addrString));
			cout << "new connection from client ip = " << addrString << ", port=" << ntohs(address.sin_port) << endl;
			
			
			//add new client to our client array:
			bool bSuccessfullyAdded{ false };
			for (int i = 0; i < max_client; ++i)
			{
				if (clientArray[i].clientSocket == 0)
				{
					clientArray[i].clientSocket = new_socket;
					clientArray[i].address = address;
					bSuccessfullyAdded = true;

					if (i == 0)
					{
						clientArray[i].name = 'X';
					}
					else if (i == 1)
					{
						clientArray[i].name = 'O';
					}
					else if (i == 2)
					{
						clientArray[i].name = 'Y';
					}

					string sendbuf;
					sendbuf.push_back(clientArray[i].name);

					send(clientArray[i].clientSocket, sendbuf.c_str(), sendbuf.size() , 0);					
					break;
				}
			}
			if (!bSuccessfullyAdded)
			{
				cout << "ran out of client_socket slot to add the new client\n";
				string sendbuf = "server full";
				send(new_socket, sendbuf.c_str(), sendbuf.size(), 0);
			}
		}

		//track how many client connect to server 
		for (int i = 0; i < max_client; ++i)
		{
			if (clientArray[i].clientSocket != 0)
			{	
				//cout << "ready ++ " << endl;
				++Ready;			
			}
			else
			{
				break;
			}
		}

		//if is 3 start game
		if (Ready == max_client && Turn < 10)
		{
			if (doOnce)
			{
				const char* sendbuf = "Is your turn bro!";
				send(clientArray[Turn % max_client].clientSocket, sendbuf, (int)strlen(sendbuf), 0);
				Turn++;
				doOnce = false;
			}

			// check if there is any activity on any of the clients:
			for (int i = 0; i < max_client; ++i)
			{
				SOCKET s = clientArray[i].clientSocket;
				if (FD_ISSET(s, &readfds))
				{
					getpeername(s, (struct sockaddr*)&address, (int*)&addrlen);
					memset(buffer, 0, sizeof(buffer));
					int charRead = recv(s, buffer, MAXRECVSIZE, 0);
					//cout << "recv buffer size=" << charRead << endl;
					char addrString[60];
					InetNtop(address.sin_family, &address.sin_addr, (PWSTR)&addrString, sizeof(addrString));
					if (charRead == SOCKET_ERROR)
					{
						int error_code{ WSAGetLastError() };
						if (error_code == WSAECONNRESET)
						{
							//a client is disconnected. Remove it from our array:
							cout << "Client ip=" << addrString << ", port=" << ntohs(address.sin_port) << " disconnected unexpectedly\n";
							closesocket(s);
							client_socket[i] = 0;
						}
						else
							cout << "recv failed with error " << error_code << endl;

					}
					else if (charRead == 0) // normal disconnection
					{
						//a client is disconnected. Remove it from our array:
						cout << "Client ip=" << addrString << ", port=" << ntohs(address.sin_port) << " disconnected.\n";
						closesocket(s);
						clientArray[i].clientSocket = 0;
					}
					else if (charRead == 10)
					{
						//cout << "confirm Message from client" << endl;
					}
					else //  dealing with messge 
					{
						ttt.Input(clientArray[i].name, buffer[0]);
						ttt.Draw();

						int num = i;
						string temp;
						temp.push_back(clientArray[i].name);
						temp.push_back(buffer[0]);
						for (int i = 0; i < max_client; ++i)
						{
							if (i != num && clientArray[i].clientSocket != INVALID_SOCKET)
							{
								send(clientArray[i].clientSocket, temp.c_str(), temp.size(), 0);
								//cout << "send move message to other client" << endl;
							}
						}
						doOnce = true;

						//track win condition
						if (ttt.CheckPlayerWin('X') || ttt.CheckPlayerWin('O') || ttt.CheckPlayerWin('Y'))
						{
							cout << "One player win the game, game end!!!" << endl;
							gameEnd = true;
							char winName;
							if (ttt.CheckPlayerWin('X'))
							{
								winName = 'X';
							}
							else if (ttt.CheckPlayerWin('O'))
							{
								winName = 'O';
							}
							else
							{
								winName = 'Y';
							}

							for (int i = 0; i < max_client; ++i)
							{
								if (clientArray[i].name == winName)
								{
									const char* temp = "You win the game!!!";
									send(clientArray[i].clientSocket, temp,strlen(temp), 0);
								}
								else
								{
									const char* temp = "You Lose!!!";
									send(clientArray[i].clientSocket, temp, strlen(temp), 0);
								}
							}

							break;
						}

						
					}
					
				}
			}
		}
		else if (Ready == max_client && Turn >= 10)
		{
			for (int i = 0; i < max_client; ++i)
			{				
				const char* temp = "You Lose!!!";
				send(clientArray[i].clientSocket, temp, strlen(temp), 0);		
			}
			break;
		}
		Ready = 0;
		//Turn++;
	}

	closesocket(listenSocket);
	WSACleanup();

	return 0;
}