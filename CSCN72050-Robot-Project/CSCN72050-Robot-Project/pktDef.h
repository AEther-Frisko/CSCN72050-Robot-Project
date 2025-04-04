/*
	File:	 pktDef.cpp
	Authors: Max Venables, Ryan Witley
	Date:	 2025-04-04
	Purpose: Defines the PktDef class.
*/

#include <iostream>
using namespace std;

const int HEADERSIZE; // size of the header in bytes -- needs to be calculated still

enum Direction {
	FORWARD = 1,
	BACKWARD = 2,
	RIGHT = 3,
	LEFT = 4
};

enum CmdType {
	DRIVE,
	SLEEP,
	RESPONSE
};

typedef struct Header {
	unsigned short int pktCount;
	unsigned short int length;
	char* data;
	unsigned char crc;
};

typedef struct DriveBody {
	enum Direction direction;
	unsigned char duration;
	unsigned short int speed;	// should stay between 80 - 100
};

class PktDef {
	typedef struct CmdPkt {
		Header header;
		char* data;
		char crc;
	};
	char* rawBuffer;

	// Default constructor
	PktDef();

	// Overloaded constructor
	PktDef(char* data);

	// Setters
	void setCmd(CmdType cmd);

	void setBodyData(char* data, int size);

	void setPktCount(int count);

	// Getters
	CmdType getCmd();

	bool getAck();

	int getlength();

	char* getBodyData();

	int getPktCount();

	/*
		Calculates the CRC based on the given raw data buffer.
		- buffer : raw data buffer
		- size	 : size of the buffer in bytes
		returns TRUE if the CRC matches the CRC of the packet in the buffer, otherwise FALSE.
	*/
	bool checkCrc(char* buffer, int size);

	/*
		Calculates the CRC and sets the object's packet CRC parameter.
	*/
	void calcCrc();

	/*
		Allocates the private raw buffer and transfers contents
		from the object's member variables into a raw data packet.
		returns address of allocated raw buffer.
	*/
	char* genPacket();
};