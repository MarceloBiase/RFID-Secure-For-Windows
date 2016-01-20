//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) 2006 Microsoft Corporation. All rights reserved.
//
// CCommandWindow provides a way to emulate external "connect" and "disconnect" 
// events, which are invoked via toggle button on a window. The window is launched
// and managed on a separate thread, which is necessary to ensure it gets pumped.
//

#pragma once

#include <list>
#include "CardUnlockProvider.h"

using namespace std;
class RfidListener
{
public:
	RfidListener(void);
	~RfidListener(void);

    HRESULT Init(CardUnlockProvider *pProvider);
    BOOL GetConnectedStatus();
	LPCWSTR UserName()
	{
		return _username;
	}
	LPCWSTR Password()
	{ 
		return _password;
	}
	void Disconnect();

private:
	static DWORD WINAPI _ThreadProc(LPVOID lpParameter);
	void waitForSerialData();
	void fetchStoredInfo();
    
	CardUnlockProvider		*_pProvider;

	string _port;

	wchar_t				_username[100];
	wchar_t				_password[100];

	BOOL				_connected;
	HANDLE              _thread;
	volatile BOOL       _quit;

	class Credential
	{
	public:
		string id;
		wchar_t username[100];
		wchar_t password[100];
	};

	list<Credential> _credentials;
};
