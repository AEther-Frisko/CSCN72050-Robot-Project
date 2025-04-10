#pragma once
/*
	File:	 mySocket.h
	Authors: Max Venables, Ryan Witley
	Date:	 2025-04-10
	Purpose: Defines the MySocket class.
*/

#include <iostream>
#include <winsock.h>
using namespace std;

const int DEFAULT_SIZE = 6;		// Default size of buffer space

enum SocketType {
	CLIENT,
	SERVER
};

enum ConnectionType {
	TCP,
	UDP
};

class MySocket {
	char* buffer;
	int welcomeSocket;
	int connectionSocket;
	struct sockaddr_in svrAddr;
	SocketType mySocket;
	string ipAddr;
	int port;
	ConnectionType connectionType;
	bool tcpConnect;
	int maxSize;

public:
	/*
		Parameterized constructor. Configures all the socket and connection information.
		- socketType     : client or server socket
		- ip             : IP address
		- port           : port number
		- connectionType : TCP or UDP connection
		- size			 : size of the buffer (replaced with DEFAULT_SIZE if invalid)
	*/
	MySocket(SocketType socketType, string ip, unsigned int port, ConnectionType connectionType, unsigned int size);

	// Destructor to clean up allocated memory.
	~MySocket();

	/*
		Establishes a TCP/IP socket connection (3-way handshake).
	*/
	void connectTCP();

	/*
		Disconnects an established TCP/IP connection (4-way handshake).
	*/
	void disconnectTCP();

	/*
		Transmits a raw block of data over the socket. Works with TCP and UDP.
		- data : raw data to be transmitted
		- size : size of the data in bytes
	*/
	void sendData(const char* data, int size);

	// === GETTERS ===

	/*
		Getter for MySocket's buffer, transfers it to the given address.
		- buffer : address to transfer the raw data to.
		returns number of bytes written.
	*/
	int getData(char* buffer);

	/*
		Getter for MySocket's ip address.
	*/
	string getIPAddr();

	/*
		Getter for MySocket's port number.
	*/
	int getPort();

	/*
		Getter for MySocket's socket type.
	*/
	SocketType getType();

	// === SETTERS ===

	/*
		Setter for MySocket's ip address.
		- ip : new ip address
		returns an error message if connection is already established.
	*/
	void setIPAddr(string ip);

	/*
		Setter for MySocket's port number.
		- port : new port number
	*/
	void setPort(int port);

	/*
		Setter for MySocket's socket type.
		- socketType : new socket type
	*/
	void setType(SocketType socketType);
};