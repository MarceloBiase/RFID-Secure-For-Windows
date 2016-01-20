//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) 2006 Microsoft Corporation. All rights reserved.
//
//

#include <tchar.h>
#include "RfidListener.h"
#include <strsafe.h>
#include "Serial.h"

using namespace std;



RfidListener::RfidListener(void)
{
	_pProvider = NULL;
	_quit = FALSE;
	_connected = FALSE;
	ZeroMemory(_username, sizeof(_username));
	ZeroMemory(_password, sizeof(_password));

	fetchStoredInfo();
}

RfidListener::~RfidListener(void)
{
	_quit = TRUE;

	WaitForSingleObject(_thread, 10000);
    // We'll also make sure to release any reference we have to the provider.
    if (_pProvider != NULL)
    {
        _pProvider->Release();
        _pProvider = NULL;
    }
}

void RfidListener::fetchStoredInfo()
{
	_port = "4";

	_credentials.push_front(Credential());

	std::list<Credential>::iterator it = _credentials.begin();
	std::advance(it, 0);
	it->id = "rfid_id_here";
	wcscpy_s(it->password, 1024, L"password_here"); // Currently hard-coded credentials
	wcscpy_s(it->username, L"username_here"); //       but eventually dynamic
}

// Performs the work required to spin off our message so we can listen for events.
HRESULT RfidListener::Init(CardUnlockProvider *pProvider)
{
    HRESULT hr = S_OK;

    // Be sure to add a release any existing provider we might have, then add a reference
    // to the provider we're working with now.
    if (_pProvider != NULL)
    {
        _pProvider->Release();
    }
    _pProvider = pProvider;
    _pProvider->AddRef();
    
    // Create and launch the window thread.
	_thread = ::CreateThread(NULL, 0, RfidListener::_ThreadProc, (LPVOID) this, 0, NULL);
	if (_thread == NULL)
    {
        hr = HRESULT_FROM_WIN32(::GetLastError());
    }

	wcscpy_s(_username, 1024, L""); // Empty username and password
	wcscpy_s(_password, 1024, L"");

    return hr;
}

BOOL RfidListener::GetConnectedStatus()
{
	return _connected;
}

void RfidListener::waitForSerialData()
{
	Serial serial = Serial();
	while (serial.Init(_port) == 0)
		Sleep(200);

	serial.Write("auth");
	serial.Read();
	if (serial.DataRead.compare("0") == 0)
	{
		serial.Write("init");
		serial.Read();
		if (serial.DataRead.compare("ok") == 0)
			serial.Write("ok");
	}

	while (!_quit)	// Ends thread on end
	{
		serial.Write("check");
		serial.Read();
		if (serial.DataRead.compare("1") == 0)
		{
			serial.Write("get");
			serial.Read();

			list<Credential>::iterator it = _credentials.begin();
			for (int i = 0; i < _credentials.size(); i++)
			{
				advance(it, 0);
				if (serial.DataRead.compare(it->id) == 0)
				{
					ZeroMemory(_username, sizeof(_username));
					ZeroMemory(_password, sizeof(_password));

					wcscpy_s(_username, 1024, it->username);
					wcscpy_s(_password, 1024, it->password);
					_connected = TRUE;
					_pProvider->OnConnectStatusChanged();
					break;
				}
			}
			if (_connected)
				break;
		}
		Sleep(1000); // Wait so as no to flood the serial port
	}
}

// Our thread procedure which waits for data on the serial port
DWORD WINAPI RfidListener::_ThreadProc(LPVOID lpParameter)
{
	RfidListener *rfidListener = static_cast<RfidListener *>(lpParameter);

	rfidListener->waitForSerialData(); // Waiting for data from Arduino

	return 0;
}

void RfidListener::Disconnect()
{
	MessageBox(NULL, ("Disconnected"), (__FUNCTION__), MB_OK | MB_TOPMOST);
	ZeroMemory(_username, sizeof(_username));
	ZeroMemory(_password, sizeof(_password));
	_pProvider->OnConnectStatusChanged();
}