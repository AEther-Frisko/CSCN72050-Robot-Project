/*
	File:	 pktDef.h
	Authors: Max Venables, Ryan Witley
	Date:	 2025-04-06
	Purpose: Defines the PktDef class.
*/

#include <iostream>
using namespace std;

const int HEADERSIZE = 6; // Size of packet header in bytes

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
	
	// Command flags
	unsigned char drive : 1;
	unsigned char status : 1;
	unsigned char sleep : 1;
	unsigned char ack : 1;
	unsigned char padding : 4; // Extra padding

	unsigned short int length;
};

typedef struct DriveBody {
	enum Direction direction;
	unsigned char duration;
	unsigned short int speed; // Should stay between 80 - 100
};

class PktDef {
	typedef struct CmdPkt {
		Header header;
		char* data;
		char crc;
	};
	CmdPkt cmdPkt;
	char* rawBuffer;

public:
	// Default constructor
	PktDef();

	/*
		Overloaded constructor. Deserializes a data buffer into a data packet.
		- buffer : raw data buffer
	*/
	PktDef(char* buffer);

	// === SETTERS ===

	void setCmd(CmdType cmd);

	void setBodyData(char* buffer, int size);

	void setPktCount(int count);

	// === GETTERS ===

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
		Helper function for calcCrc and checkCrc. Counts set bits in a given byte.
		- byte : byte to be analyzed
		returns the number of set bits.
	*/
	int countSetBits(unsigned char byte);

	/*
		Allocates the private raw buffer and transfers contents
		from the object's member variables into a raw data packet.
		returns address of allocated raw buffer.
	*/
	char* genPacket();
};