
#define WIN32_LEAN_AND_MEAN
#include"TicTacToe.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
using namespace std;

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 8889

int main()
{
	TicTacToe ttt;
	char name;
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;
	const char* sendbuf = "this is a test from client";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Validate the parameters
	/*if (argc < 2) {
		printf("usage: %s server-name\n", argv[0]);
		return 1;
	}*/

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	struct sockaddr_in servaddr;
	ZeroMemory(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	//servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	string ip = "127.0.0.2";
	//InetPton(AF_INET, argv[1], &servaddr.sin_addr.s_addr);
	inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr);
	servaddr.sin_port = htons(DEFAULT_PORT);

	iResult = connect(ConnectSocket, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		return 1;
	}

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	//get name for game
	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	if (strcmp(recvbuf, "server full") == 0)
	{
		cout << "server is full, try later!" << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	else
	{
		name = recvbuf[0];
		cout << "Your name is: " <<name<<"  wait for other players!!"<< endl;
		//recvbuf[iResult] = '\0';
	}


	do {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		//cout << "iResult value is: " << iResult << endl;
		if (iResult > 0)
		{
			recvbuf[iResult] = '\0';
			//printf("Bytes received: %d: %s\n", iResult, recvbuf);
		
			if (strcmp(recvbuf, "Is your turn bro!") == 0)
			{
				while(1)
				{
					ttt.Draw();
					cout << "Is your turn bro!" << endl;
					std::string sendvBuf{ "" };
					std::getline(std::cin, sendvBuf);

					if (ttt.Input(name, sendvBuf[0]) && sendvBuf.size() == 1)
					{
						iResult = send(ConnectSocket, sendvBuf.c_str(), (int)strlen(sendvBuf.c_str()), 0);
						break;
						if (iResult == SOCKET_ERROR)
						{
							printf("send failed with error: %d\n", WSAGetLastError());
							closesocket(ConnectSocket);
							WSACleanup();
							return 1;
						}
					}
					else
					{
						cout << "invalid input, try again!" << endl;
						continue;
					}
				}
			}
			else if(iResult == 2)//message from other client
			{
				//cout << "Other players move done" << endl;
				string temp = recvbuf;
				ttt.Input(temp[0], temp[1]);
				//ttt.Draw();
				//confirm message
				send(ConnectSocket, "confirmmm", 10, 0);
			}
			else if (strcmp(recvbuf, "You win the game!!!") == 0 || strcmp(recvbuf, "You Lose!!!") == 0)
			{
				ttt.Draw();
				cout << recvbuf << endl;
				cout << "Game ended" << endl;
				break;
			}
		}
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}

