#include "Serial.h"
#include <iostream>

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	Serial serial = Serial();

	while (serial.Init("4") == 0)
		Sleep(200);

	serial.Write("auth");
	if (serial.Read() == 0)
	{
		LockWorkStation();
		return 0;
	}
	if (serial.DataRead.compare("0") == 0)
	{
		serial.Write("init");
		if (serial.Read() == 0)
		{
			LockWorkStation();
			return 0;
		}
		if (serial.DataRead.compare("ok") == 0)
			serial.Write("ok");
	}

	do
	{
		serial.Write("check");
		if (serial.Read() == 0)
		{
			LockWorkStation();
			return 0;
		}
		if (serial.DataRead.compare("1") == 0)
		{
			serial.Write("get");
			if (serial.Read() == 0)
			{
				LockWorkStation();
				return 0;
			}
			if (serial.DataRead.compare("a587a009") != 0)
			{
				if (LockWorkStation())
					break;
			}
		}
		else
		{
			if (LockWorkStation())
				break;
		}
		Sleep(1000);
	} while (true);
}