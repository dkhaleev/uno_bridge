#include "SDRunoPlugin_RemoteBridgeSerial.h"



//_____________________________________
// ::: Constructors and destructors :::


/*!
	\brief      Constructor of the class rOc_serial.
*/
Serial::Serial()
{}


/*!
	\brief      Destructor of the class rOc_serial. It close the connection
*/
// Class desctructor
Serial::~Serial()
{
	closeDevice();
}



//_________________________________________
// ::: Configuration and initialization :::



/*!
	 \brief Open the serial port
	 \param Device : Port name (COM1, COM2, ... for Windows ) or (/dev/ttyS0, /dev/ttyACM0, /dev/ttyUSB0 ... for linux)
	 \param Bauds : Baud rate of the serial port.

				\n Supported baud rate for Windows :
						- 110
						- 300
						- 600
						- 1200
						- 2400
						- 4800
						- 9600
						- 14400
						- 19200
						- 38400
						- 56000
						- 57600
						- 115200
						- 128000
						- 256000

			   \n Supported baud rate for Linux :\n
						- 110
						- 300
						- 600
						- 1200
						- 2400
						- 4800
						- 9600
						- 19200
						- 38400
						- 57600
						- 115200

	 \return 1 success
	 \return -1 device not found
	 \return -2 error while opening the device
	 \return -3 error while getting port parameters
	 \return -4 Speed (Bauds) not recognized
	 \return -5 error while writing port parameters
	 \return -6 error while writing timeout parameters
  */
char Serial::openDevice(const char *Device, const unsigned int Bauds)
{
	// Open serial port
	//hSerial = CreateFileA(Device, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	LPCTSTR pcCommPortWin32DevicePath = TEXT("\\\\.\\COM48");
	//LPCTSTR pcCommPortWin32DevicePath = L"\\\\.\\COM48";
	hSerial = ::CreateFile(pcCommPortWin32DevicePath, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//hSerial = CreateFile(pcCommPortWin32DevicePath,
	//                       GENERIC_READ | GENERIC_WRITE,
	//                       0,      //  must be opened with exclusive-access
	//                       NULL,   //  default security attributes
	//                       OPEN_EXISTING, //  must use OPEN_EXISTING
	//                       0,      //  not overlapped I/O
	//                       NULL); //  hRemoteBridge must be NULL for comm devices
	/*
	CreateFile(TEXT("\\\\.\\COM48"),
		GENERIC_READ | GENERIC_WRITE,
		0,      //  must be opened with exclusive-access
		NULL,   //  default security attributes
		OPEN_EXISTING, //  must use OPEN_EXISTING
		0,      //  not overlapped I/O
		NULL);
	*/
	if (hSerial == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			return -1;
		}// Device not found
		return -2;                                                      // Error while opening the device
	}

	// Set parameters
	DCB dcbSerialParams = { 0 };                                          // Structure for the port parameters


	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams))                       // Get the port parameters
		return -3;                                                      // Error while getting port parameters
	switch (Bauds)                                                      // Set the speed (Bauds)
	{
	case 110:     dcbSerialParams.BaudRate = CBR_110; break;
	case 300:     dcbSerialParams.BaudRate = CBR_300; break;
	case 600:     dcbSerialParams.BaudRate = CBR_600; break;
	case 1200:     dcbSerialParams.BaudRate = CBR_1200; break;
	case 2400:     dcbSerialParams.BaudRate = CBR_2400; break;
	case 4800:     dcbSerialParams.BaudRate = CBR_4800; break;
	case 9600:     dcbSerialParams.BaudRate = CBR_9600; break;
	case 14400:    dcbSerialParams.BaudRate = CBR_14400; break;
	case 19200:    dcbSerialParams.BaudRate = CBR_19200; break;
	case 38400:    dcbSerialParams.BaudRate = CBR_38400; break;
	case 56000:    dcbSerialParams.BaudRate = CBR_56000; break;
	case 57600:    dcbSerialParams.BaudRate = CBR_57600; break;
	case 115200:   dcbSerialParams.BaudRate = CBR_115200; break;
	case 128000:   dcbSerialParams.BaudRate = CBR_128000; break;
	case 256000:   dcbSerialParams.BaudRate = CBR_256000; break;
	default: return -4;
	}
	dcbSerialParams.ByteSize = 8;                                         // 8 bit data
	dcbSerialParams.StopBits = ONESTOPBIT;                                // One stop bit
	dcbSerialParams.Parity = NOPARITY;                                    // No parity
	if (!SetCommState(hSerial, &dcbSerialParams))                        // Write the parameters
		return -5;                                                      // Error while writing

	// Set TimeOut
	timeouts.ReadIntervalTimeout = 0;                                     // Set the Timeout parameters
	timeouts.ReadTotalTimeoutConstant = MAXDWORD;                         // No TimeOut
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = MAXDWORD;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	if (!SetCommTimeouts(hSerial, &timeouts)){                            // Write the parameters
		return -6;                                                      // Error while writting the parameters
	}

	return 1;                                                           // Opening successfull
}


/*!
	 \brief Close the connection with the current device
*/
void Serial::closeDevice()
{
	CloseHandle(hSerial);
}




//___________________________________________
// ::: Read/Write operation on characters :::



/*!
	 \brief Write a char on the current serial port
	 \param Byte : char to send on the port (must be terminated by '\0')
	 \return 1 success
	 \return -1 error while writting data
  */
char Serial::writeChar(const char Byte)
{
	DWORD dwBytesWritten;                                               // Number of bytes written
	if (!WriteFile(hSerial, &Byte, 1, &dwBytesWritten, NULL))                // Write the char
		return -1;                                                      // Error while writing
	return 1;                                                           // Write operation successfull
}



//________________________________________
// ::: Read/Write operation on strings :::


/*!
	 \brief     Write a string on the current serial port
	 \param     String : string to send on the port (must be terminated by '\0')
	 \return     1 success
	 \return    -1 error while writting data
  */
char Serial::writeString(const char *String)
{
	DWORD dwBytesWritten;                                               // Number of bytes written
	if (!WriteFile(hSerial, String, strlen(String), &dwBytesWritten, NULL))  // Write the string
		return -1;                                                      // Error while writing
	return 1;                                                           // Write operation successfull
}

// _____________________________________
// ::: Read/Write operation on bytes :::



/*!
	 \brief Write an array of data on the current serial port
	 \param Buffer : array of bytes to send on the port
	 \param NbBytes : number of byte to send
	 \return 1 success
	 \return -1 error while writting data
  */
char Serial::writeBytes(const void *Buffer, const unsigned int NbBytes)
{
	DWORD dwBytesWritten;                                               // Number of byte written
	if (!WriteFile(hSerial, Buffer, NbBytes, &dwBytesWritten, NULL))     // Write data
		return -1;                                                      // Error while writing
	return 1;                                                           // Write operation successfull
}



/*!
	 \brief Wait for a byte from the serial device and return the data read
	 \param pByte : data read on the serial device
	 \param TimeOut_ms : delay of timeout before giving up the reading
			If set to zero, timeout is disable (Optional)
	 \return 1 success
	 \return 0 Timeout reached
	 \return -1 error while setting the Timeout
	 \return -2 error while reading the byte
  */
char Serial::readChar(char *pByte, unsigned int TimeOut_ms)
{
	DWORD dwBytesRead = 0;
	timeouts.ReadTotalTimeoutConstant = TimeOut_ms;                       // Set the TimeOut
	if (!SetCommTimeouts(hSerial, &timeouts))                            // Write the parameters
		return -1;                                                      // Error while writting the parameters
	if (!ReadFile(hSerial, pByte, 1, &dwBytesRead, NULL))                 // Read the byte
		return -2;                                                      // Error while reading the byte
	if (dwBytesRead == 0) return 0;                                       // Return 1 if the timeout is reached
	return 1;                                                           // Success
}



/*!
	 \brief Read a string from the serial device (without TimeOut)
	 \param String : string read on the serial device
	 \param FinalChar : final char of the string
	 \param MaxNbBytes : maximum allowed number of bytes read
	 \return >0 success, return the number of bytes read
	 \return -1 error while setting the Timeout
	 \return -2 error while reading the byte
	 \return -3 MaxNbBytes is reached
  */
int Serial::readStringNoTimeOut(char *String, char FinalChar, unsigned int MaxNbBytes)
{
	unsigned int    NbBytes = 0;                                          // Number of bytes read
	char            ret;                                                // Returned value from Read
	while (NbBytes < MaxNbBytes)                                          // While the buffer is not full
	{                                                                   // Read a byte with the restant time
		ret = readChar(&String[NbBytes]);
		if (ret == 1)                                                     // If a byte has been read
		{
			if (String[NbBytes] == FinalChar)                             // Check if it is the final char
			{
				String[++NbBytes] = 0;                                  // Yes : add the end character 0
				return NbBytes;                                         // Return the number of bytes read
			}
			NbBytes++;                                                  // If not, just increase the number of bytes read
		}
		if (ret < 0) return ret;                                          // Error while reading : return the error number
	}
	return -3;                                                          // Buffer is full : return -3
}

/*!
	 \brief Read a string from the serial device (with timeout)
	 \param String : string read on the serial device
	 \param FinalChar : final char of the string
	 \param MaxNbBytes : maximum allowed number of bytes read
	 \param TimeOut_ms : delay of timeout before giving up the reading (optional)
	 \return  >0 success, return the number of bytes read
	 \return  0 timeout is reached
	 \return -1 error while setting the Timeout
	 \return -2 error while reading the byte
	 \return -3 MaxNbBytes is reached
  */
int Serial::readString(char *String, char FinalChar, unsigned int MaxNbBytes, unsigned int TimeOut_ms)
{
	if (TimeOut_ms == 0)
		return readStringNoTimeOut(String, FinalChar, MaxNbBytes);

	unsigned int    NbBytes = 0;                                          // Number of bytes read
	char            ret;                                                // Returned value from Read
	TimeOut         Timer;                                              // Timer used for timeout
	long int        TimeOutParam;

	Timer.InitTimer();                                                  // Initialize the timer

	while (NbBytes < MaxNbBytes)                                          // While the buffer is not full
	{                                                                   // Read a byte with the restant time
		TimeOutParam = TimeOut_ms - Timer.ElapsedTime_ms();                 // Compute the TimeOut for the call of ReadChar
		if (TimeOutParam > 0)                                             // If the parameter is higher than zero
		{
			ret = readChar(&String[NbBytes], TimeOutParam);                // Wait for a byte on the serial link
			if (ret == 1)                                                 // If a byte has been read
			{

				if (String[NbBytes] == FinalChar)                         // Check if it is the final char
				{
					String[++NbBytes] = 0;                              // Yes : add the end character 0
					return NbBytes;                                     // Return the number of bytes read
				}
				NbBytes++;                                              // If not, just increase the number of bytes read
			}
			if (ret < 0) return ret;                                      // Error while reading : return the error number
		}
		if (Timer.ElapsedTime_ms() > TimeOut_ms) {                        // Timeout is reached
			String[NbBytes] = 0;                                          // Add the end caracter
			return 0;                                                   // Return 0
		}
	}
	return -3;                                                          // Buffer is full : return -3
}


/*!
	 \brief Read an array of bytes from the serial device (with timeout)
	 \param Buffer : array of bytes read from the serial device
	 \param MaxNbBytes : maximum allowed number of bytes read
	 \param TimeOut_ms : delay of timeout before giving up the reading
	 \return >=0 return the number of bytes read before timeout or
				requested data is completed
	 \return -1 error while setting the Timeout
	 \return -2 error while reading the byte
  */
int Serial::readBytes(void *Buffer, unsigned int MaxNbBytes, unsigned int TimeOut_ms, unsigned int SleepDuration_us)
{
	DWORD dwBytesRead = 0;
	timeouts.ReadTotalTimeoutConstant = (DWORD)TimeOut_ms;                // Set the TimeOut
	if (!SetCommTimeouts(hSerial, &timeouts))                            // Write the parameters
		return -1;                                                      // Error while writting the parameters
	if (!ReadFile(hSerial, Buffer, (DWORD)MaxNbBytes, &dwBytesRead, NULL))  // Read the bytes from the serial device
		return -2;                                                      // Error while reading the byte
	return dwBytesRead;
}




// _________________________
// ::: Special operation :::



/*!
	\brief Empty receiver buffer (UNIX only)
*/

void Serial::flushReceiver()
{

}

/*!
	\brief  Return the number of bytes in the received buffer (UNIX only)
	\return The number of bytes in the received buffer
*/
int Serial::peekReceiver()
{
	DWORD   errors = CE_IOE;
	COMSTAT commStat;
	int Nbytes = 0;

	if (!ClearCommError(hSerial, &errors, &commStat))
		Nbytes = 0;
	else
		Nbytes = commStat.cbInQue;

	return Nbytes;
}

byte Serial::crc8(byte *buffer, byte size)
{
	byte crc = 0;
	for (byte i = 0; i < size; i++) {
		byte data = buffer[i];
		for (int j = 8; j > 0; j--) {
			crc = ((crc ^ data) & 1) ? (crc >> 1) ^ 0x8C : (crc >> 1);
			data >>= 1;
		}
	}

	return crc;
}