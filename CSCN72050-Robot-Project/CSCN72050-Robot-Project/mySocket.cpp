/*
	File:	 mySocket.cpp
	Authors: Max Venables, Ryan Witley
	Date:	 2025-04-10
	Purpose: Implementation of the MySocket class functions.
*/

#include "mySocket.h"

int MySocket::getData(char* buffer) {
	memcpy(this->buffer, buffer, sizeof(buffer));
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
	// TODO: send error if already connected
	ipAddr = ip;
}

void MySocket::setPort(int port) {
	this->port = port;
}

void MySocket::setType(SocketType socketType) {
	this->mySocket = socketType;
}