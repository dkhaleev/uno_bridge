#pragma once
#ifndef ROC_SERIAL_H
#define ROC_SERIAL_H

#include <Windows.h>
#include "SDRunoPlugin_RemoteBridgeSerialTimeout.h"

class Serial
{
public:
	//Constructor
	Serial();
	//Destructor
	~Serial();

	//_________________________________________
	// ::: Configuration and initialization :::


	// Open a device
	char    openDevice(const char *Device, const unsigned int Bauds);

	// Close the current device
	void    closeDevice();

	//___________________________________________
	// ::: Read/Write operation on characters :::


	// Write a char
	char    writeChar(char);

	// Read a char (with timeout)
	char    readChar(char *pByte, const unsigned int TimeOut_ms = 0);


	//________________________________________
	// ::: Read/Write operation on strings :::


	// Write a string
	char    writeString(const char *String);
	// Read a string (with timeout)
	int     readString(char *String,
		char FinalChar,
		unsigned int MaxNbBytes,
		const unsigned int TimeOut_ms = 0);



	// _____________________________________
	// ::: Read/Write operation on bytes :::


	// Write an array of bytes
	char    writeBytes(const void *Buffer, const unsigned int NbBytes);

	// Read an array of byte (with timeout)
	int     readBytes(void *Buffer, unsigned int MaxNbBytes, const unsigned int TimeOut_ms = 0, unsigned int SleepDuration_us = 100);

	// _________________________
	// ::: Special operation :::


	// Empty the received buffer
	void    flushReceiver();

	// Return the number of bytes in the received buffer
	int     peekReceiver();


	// _________________________
	// ::: Access to IO bits :::


	// Change CTR status
	void    DTR(bool Status);

	// Change RTS status
	void    RTS(bool Status);

	// Get CTS bit
	bool    isCTS();

	// Get DTR bit
	bool    isDTR();

	// Get CTS bit
	bool    isRTS();

	byte crc8(byte *buffer, byte size);

private:
	// Read a string (no timeout)
	int     readStringNoTimeOut(char *String, char FinalChar, unsigned int MaxNbBytes);

	HANDLE          hSerial;
	COMMTIMEOUTS    timeouts;

	byte crc;
};
#endif // ROC_SERIAL_H 