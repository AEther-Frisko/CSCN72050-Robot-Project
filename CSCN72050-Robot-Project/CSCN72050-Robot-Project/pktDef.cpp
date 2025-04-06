/*
	File:	 pktDef.cpp
	Authors: Max Venables, Ryan Witley
	Date:	 2025-04-06
	Purpose: Implementation of the PktDef class functions.
*/

#include "pktDef.h"

PktDef::PktDef() {
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

PktDef::PktDef(char* buffer) {
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

void PktDef::setCmd(CmdType cmd) {
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

void PktDef::setBodyData(char* buffer, int size) {
	cmdPkt.data = new char[size];
	memcpy(cmdPkt.data, buffer, size);
}

void PktDef::setPktCount(int count) {
	cmdPkt.header.pktCount = count;
}

CmdType PktDef::getCmd() {
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

bool PktDef::getAck() {
	if (cmdPkt.header.ack == 1) {
		return true;
	}
	else {
		return false;
	}
}

int PktDef::getlength() {
	return cmdPkt.header.length;
}

char* PktDef::getBodyData() {
	return cmdPkt.data;
}

int PktDef::getPktCount() {
	return cmdPkt.header.pktCount;
}

bool PktDef::checkCrc(char* buffer, int size) {
	if (size <= 1) return false;

	char computedCRC = 0;
	for (int i = 0; i < size - 1; ++i) {
		computedCRC += countSetBits(static_cast<unsigned char>(buffer[i]));
	}

	char actualCRC = buffer[size - 1];
	return computedCRC == actualCRC;
}

void PktDef::calcCrc() {
	size_t dataLen = cmdPkt.header.length - HEADERSIZE;

	// Excludes the CRC
	char* buffer = new char[cmdPkt.header.length - 1];
	size_t offset = 0;

	// Ensuring proper size to avoid overflow
	if (cmdPkt.header.length < HEADERSIZE) {
		cout << "ERROR: invalid header length." << endl;
	}
	else {
		// Serializes fields for calculation (aside from the CRC)
		memcpy(buffer + offset, &cmdPkt.header.pktCount, sizeof(cmdPkt.header.pktCount));
		offset += sizeof(cmdPkt.header.pktCount);

		unsigned char cmd =
			(cmdPkt.header.drive << 0) |
			(cmdPkt.header.status << 1) |
			(cmdPkt.header.sleep << 2) |
			(cmdPkt.header.ack << 3);
		memcpy(buffer + offset, &cmd, sizeof(cmd));
		offset += sizeof(cmd);

		memcpy(buffer + offset, &cmdPkt.header.length, sizeof(cmdPkt.header.length));
		offset += sizeof(cmdPkt.header.length);

		memcpy(buffer + offset, cmdPkt.data, dataLen);
		offset += dataLen;

		// Computes CRC via the buffer
		cmdPkt.crc = 0;
		for (size_t i = 0; i < offset; ++i) {
			cmdPkt.crc += countSetBits(static_cast<unsigned char>(buffer[i]));
		}

		delete[] buffer;
	}
}

int PktDef::countSetBits(unsigned char byte) {
	int count = 0;
	while (byte) {
		count += byte & 1;
		byte >>= 1;
	}
	return count;
}

char* PktDef::genPacket() {
	// Ensures the header is an appropriate size
	if (cmdPkt.header.length < HEADERSIZE) {
		cout << "ERROR: invalid header length." << endl;
		return nullptr;
	}

	// Allocates buffer to store raw data
	char* buffer = new char[cmdPkt.header.length];
	size_t offset = 0;

	// Stores the packet count
	memcpy(buffer + offset, &cmdPkt.header.pktCount, sizeof(cmdPkt.header.pktCount));
	offset += sizeof(cmdPkt.header.pktCount);

	// Stores all the command flags
	unsigned char cmdByte =
		(cmdPkt.header.drive << 0) |
		(cmdPkt.header.status << 1) |
		(cmdPkt.header.sleep << 2) |
		(cmdPkt.header.ack << 3);
	memcpy(buffer + offset, &cmdByte, sizeof(cmdByte));

	// Stores the packet length
	memcpy(buffer + offset, &cmdPkt.header.length, sizeof(cmdPkt.header.length));

	// Stores the packet data
	size_t dataLen = cmdPkt.header.length - HEADERSIZE;
	memcpy(buffer + offset, cmdPkt.data, dataLen);
	offset += dataLen;

	// Stores the CRC
	memcpy(buffer + offset, &cmdPkt.crc, sizeof(cmdPkt.crc));

	return buffer;
}