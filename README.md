#RFID Secure For Windows
This is a project that allows an Arduino and RFID reader to be used with an RFID tag to control computer access
There are 3 total projects that make up this one

1. CardUnlock - The credential provider for unlocking the computer
2. CardLock - The program started by task scheduler to check if the card is removed
3. CardRead - The arduino code that allows communication with the software

CardUnlock and CardLock both build in visual studio

The output of both should be added to the system32 folder

Register.reg then needs to be run as admin and AddTask.bat does as well
