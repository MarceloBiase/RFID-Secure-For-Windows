#include "windows.h"
#include "tchar.h"
#include "strsafe.h"
#include <string>

#define MAXLEN 15

using namespace std;

class Serial
{
	public:
		Serial(void);
		~Serial(void);

		int Init(string Port);
		int Read();
		int Write(string Data);

		string DataRead;

	private:
		char endTerm = '\n';
		int count = 0;
		HANDLE hCom;
};