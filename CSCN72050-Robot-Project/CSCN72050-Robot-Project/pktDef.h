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
		size_t offset = 0;

		// Read packet count (2 bytes)
		memcpy(&cmdPkt.header.pktCount, buffer + offset, sizeof(cmdPkt.header.pktCount));
		offset += sizeof(cmdPkt.header.pktCount);

		// Read command flags and padding (1 byte total)
		unsigned char cmdByte;
		memcpy(&cmdByte, buffer + offset, sizeof(cmdByte));
		offset += sizeof(cmdByte);

		cmdPkt.header.drive = (cmdByte >> 0) & 0x01;
		cmdPkt.header.status = (cmdByte >> 1) & 0x01;
		cmdPkt.header.sleep = (cmdByte >> 2) & 0x01;
		cmdPkt.header.ack = (cmdByte >> 3) & 0x01;
		cmdPkt.header.padding = 0;

		// Read Length (2 bytes)
		memcpy(&cmdPkt.header.length, buffer + offset, sizeof(cmdPkt.header.length));
		offset += sizeof(cmdPkt.header.length);

		// Allocate and copy Data (size varies)
		size_t dataLen = cmdPkt.header.length - (sizeof(cmdPkt.header.pktCount) + sizeof(cmdByte) + sizeof(cmdPkt.header.length) + sizeof(cmdPkt.crc));
		cmdPkt.data = new char[dataLen];
		std::memcpy(cmdPkt.data, buffer + offset, dataLen);
		offset += dataLen;

		// Read CRC (1 byte)
		memcpy(&cmdPkt.crc, buffer + offset, sizeof(cmdPkt.crc));
	}

	// === SETTERS ===

	void setCmd(CmdType cmd) {
		// Preventing multiple flags from being active at once
		cmdPkt.header.drive = 0;
		cmdPkt.header.status = 0;
		cmdPkt.header.sleep = 0;

		// Activate flag accordingly
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
	char* genPacket() {
		// Ensure that the header is an appropriate size
		constexpr size_t HEADER_SIZE = sizeof(cmdPkt.header.pktCount) + sizeof(unsigned char) + sizeof(cmdPkt.header.length) + sizeof(cmdPkt.crc);
		if (cmdPkt.header.length < HEADER_SIZE) {
			cout << "ERROR: invalid header length." << endl;
			return nullptr;
		}

		// Allocate buffer to store raw data
		char* buffer = new char[cmdPkt.header.length];
		size_t offset = 0;

		// Store the packet count
		memcpy(buffer + offset, &cmdPkt.header.pktCount, sizeof(cmdPkt.header.pktCount));
		offset += sizeof(cmdPkt.header.pktCount);

		// Store all the command flags
		unsigned char cmdByte =
			(cmdPkt.header.drive << 0) |
			(cmdPkt.header.status << 1) |
			(cmdPkt.header.sleep << 2) |
			(cmdPkt.header.ack << 3);
		memcpy(buffer + offset, &cmdByte, sizeof(cmdByte));

		// Store the packet length
		memcpy(buffer + offset, &cmdPkt.header.length, sizeof(cmdPkt.header.length));

		// Store the packet data
		size_t dataLen = cmdPkt.header.length - HEADER_SIZE;
		memcpy(buffer + offset, cmdPkt.data, dataLen);
		offset += dataLen;

		// Store the CRC
		memcpy(buffer + offset, &cmdPkt.crc, sizeof(cmdPkt.crc));

		return buffer;
	}
};