#include "Serial.h"

using namespace std;

Serial::Serial(void)
{
	hCom = NULL;
}

Serial::~Serial(void)
{

}

int Serial::Init(string Port)
{
	Port = "\\\\.\\COM" + Port;

	// open the com port
	hCom = CreateFile(Port.c_str(), //RFID_Port should only contain "COM3" or "COM2" etc.
		GENERIC_READ | GENERIC_WRITE,	 // we are only reading from the COM port
		0,               // exclusive access
		NULL,            // no security
		OPEN_EXISTING,
		0,               // no overlapped I/O
		NULL);           // null template 

	if (hCom == INVALID_HANDLE_VALUE)
		return ERROR;

	// initialize port with the correct baud rate, etc.
	DCB  dcb;
	FillMemory(&dcb, sizeof(dcb), 0);

	BOOL bResult = FALSE;

	if (GetCommState(hCom, &dcb))
	{
		dcb.BaudRate = CBR_9600;
		dcb.fBinary = TRUE;
		dcb.ByteSize = 8;
		dcb.fParity = FALSE;
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;
		// TODO: flow control is currently disabled, this can be turned on
		// if the RFID reader uses it
		dcb.fDtrControl = DTR_CONTROL_DISABLE;
		dcb.fRtsControl = RTS_CONTROL_DISABLE;

		if (SetCommState(hCom, &dcb))
		{
			COMMTIMEOUTS CommTimeouts = { 0 };

			CommTimeouts.ReadIntervalTimeout = 100000 / 9600;	// 10x intercharacter timing
			CommTimeouts.ReadTotalTimeoutConstant = 1000;		// 1 second timeout overall
			CommTimeouts.ReadTotalTimeoutMultiplier = 0;

			if (SetCommTimeouts(hCom, &CommTimeouts))
			{
				bResult = TRUE;
			}
		}
		return bResult;
	}

	return 0;
}

int Serial::Read()
{
	if (hCom != NULL)
	{
		DataRead = "";

		char val;
		DWORD nBytesRead;
		int total = 0;
		while (true)
		{
			ReadFile(hCom, &val, 1, &nBytesRead, NULL);
			if (nBytesRead > 0)
			{
				if (val == endTerm)
					break;

				DataRead += val;
				total += nBytesRead;
			}
		}
		return total;
	}

	return 0;
}

int Serial::Write(string Data)
{
	Data += endTerm;

	DWORD nBytesWritten;
	if (hCom != NULL)
	{
		WriteFile(hCom, Data.c_str(), strlen(Data.c_str()), &nBytesWritten, NULL);
		return nBytesWritten;
	}

	return 0;
}