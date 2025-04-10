/*
	File:	 mySocket.cpp
	Authors: Max Venables, Ryan Witley
	Date:	 2025-04-10
	Purpose: Implementation of the MySocket class functions.
*/

#include "mySocket.h"

MySocket::MySocket(SocketType socketType, string ip, unsigned int port, ConnectionType connectionType, unsigned int size) {
	mySocket = socketType;
	ipAddr = ip;
	this->port = port;
	this->connectionType = connectionType;
	tcpConnect = false;
	
	if (size == 0) {
		buffer = new char[DEFAULT_SIZE];
		maxSize = DEFAULT_SIZE;
	}
	else {
		buffer = new char[size];
		maxSize = size;
	}

	//starts Winsock DLLs		
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		cout << "ERROR: WSAStartup failure" << endl;

	// Setting up server address info
	svrAddr.sin_family = AF_INET;
	svrAddr.sin_addr.s_addr = inet_addr(ipAddr.c_str());
	svrAddr.sin_port = htons(this->port);

	// default initializations
	connectionSocket = SOCKET_ERROR;
	welcomeSocket = SOCKET_ERROR;

	switch (connectionType)
	{
	case TCP:
		switch (socketType) {
		case SERVER:
			// initialize welcome socket
			welcomeSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (welcomeSocket == INVALID_SOCKET) {
				WSACleanup();
			}

			// bind welcome socket
			if (bind(welcomeSocket, (struct sockaddr*)&svrAddr, sizeof(svrAddr)) == SOCKET_ERROR)
			{
				closesocket(welcomeSocket);
				WSACleanup();
			}
			break;
		case CLIENT:
			// initialize connection socket
			connectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (connectionSocket == INVALID_SOCKET) {
				WSACleanup();
			}
			break;
		default:
			break;
		}
		break;
	case UDP:
		// initialize connection socket
		connectionSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (connectionSocket == INVALID_SOCKET) {
			WSACleanup();
		}
		if (socketType == SERVER) {
			// bind connection socket
			if (bind(connectionSocket, (struct sockaddr*)&svrAddr, sizeof(svrAddr)) == SOCKET_ERROR)
			{
				closesocket(connectionSocket);
				WSACleanup();
			}
		}
		break;
	default:
		cout << "ERROR: invalid connection type" << endl;
		break;
	}
}

MySocket::~MySocket() {
	closesocket(connectionSocket);
	closesocket(welcomeSocket);
	WSACleanup();
	delete[] buffer;
}

int MySocket::getData(char* buffer) {
	memcpy(this->buffer, buffer, maxSize);
	return sizeof(this->buffer);
}

string MySocket::getIPAddr() {
	return ipAddr;
}

int MySocket::getPort() {
	return port;
}

SocketType MySocket::getType() {
	return mySocket;
}

void MySocket::setIPAddr(string ip) {
	if (tcpConnect) {
		cout << "ERROR setting IP: already connected." << endl;
	}
	else {
		ipAddr = ip;
	}
}

void MySocket::setPort(int port) {
	this->port = port;
}

void MySocket::setType(SocketType socketType) {
	this->mySocket = socketType;
}