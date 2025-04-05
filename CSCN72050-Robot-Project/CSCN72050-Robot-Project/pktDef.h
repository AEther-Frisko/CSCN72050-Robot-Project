/*
	File:	 pktDef.h
	Authors: Max Venables, Ryan Witley
	Date:	 2025-04-04
	Purpose: Defines the PktDef class.
*/

#include <iostream>
using namespace std;

const int HEADERSIZE = 4; // Size of packet header in bytes

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
	PktDef() {
		cmdPkt.header.pktCount = 0;
		cmdPkt.header.drive = 0;
		cmdPkt.header.status = 0;
		cmdPkt.header.sleep = 0;
		cmdPkt.header.ack = 0;
		cmdPkt.header.padding = 0;
		cmdPkt.header.length = 0;
		cmdPkt.data = nullptr;
		cmdPkt.crc = 0;
		rawBuffer = nullptr;
	}

	/*
		Overloaded constructor. Deserializes a data buffer into a data packet.
		- buffer : raw data buffer
	*/
	PktDef(char* buffer) {
		int offset = 0;
		
		// Read packet count (2 bytes)
		cmdPkt.header.pktCount = *(reinterpret_cast<const uint16_t*>(buffer + offset));
		offset += 2;

		// Read command flags and padding (1 byte total)
		unsigned char flags = *(reinterpret_cast<const unsigned char*>(buffer + offset));
		cmdPkt.header.drive = (flags >> 0) & 0x01;
		cmdPkt.header.status = (flags >> 1) & 0x01;
		cmdPkt.header.sleep = (flags >> 2) & 0x01;
		cmdPkt.header.ack = (flags >> 3) & 0x01;
		cmdPkt.header.padding = (flags >> 4) & 0x0F;
		offset += 1;

		// Read Length (2 bytes)
		cmdPkt.header.length = *(reinterpret_cast<const uint16_t*>(buffer + offset));
		offset += 2;

		// Allocate and copy Data (size varies)
		cmdPkt.data = new char[cmdPkt.header.length];
		memcpy(cmdPkt.data, buffer + offset, cmdPkt.header.length);
		offset += cmdPkt.header.length;

		// Read CRC (1 byte)
		cmdPkt.crc = *(reinterpret_cast<const unsigned char*>(buffer + offset));
	}

	// === SETTERS ===

	void setCmd(CmdType cmd) {
		// Preventing multiple flags from being active at once
		cmdPkt.header.drive = 0;
		cmdPkt.header.status = 0;
		cmdPkt.header.sleep = 0;

		switch (cmd) {
		case DRIVE:
			cmdPkt.header.drive = 1;
			break;
		case SLEEP:
			cmdPkt.header.sleep = 1;
			break;
		case RESPONSE:
			cmdPkt.header.status = 1;
			break;
		}
	}

	void setBodyData(char* buffer, int size) {
		cmdPkt.data = new char[size];
		memcpy(cmdPkt.data, buffer, size);
	}

	void setPktCount(int count) {
		cmdPkt.header.pktCount = count;
	}

	// === GETTERS ===

	CmdType getCmd() {
		if (cmdPkt.header.drive == 1) {
			return DRIVE;
		}
		else if (cmdPkt.header.sleep == 1) {
			return SLEEP;
		}
		else if (cmdPkt.header.status == 1) {
			return RESPONSE;
		}
	}

	bool getAck() {
		if (cmdPkt.header.ack == 1) {
			return true;
		}
		else {
			return false;
		}
	}

	int getlength() {
		return cmdPkt.header.length;
	}

	char* getBodyData() {
		return cmdPkt.data;
	}

	int getPktCount() {
		return cmdPkt.header.pktCount;
	}

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