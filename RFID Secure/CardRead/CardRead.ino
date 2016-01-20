 /* 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>
#include "MFRC522.h"

#define RST_PIN		9		// 
#define SS_PIN		10		//

MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance

char endTerm = '\n';

char serRead[15];
int count;
bool auth = false;

void setup() {
  Serial.begin(9600);		// Initialize serial communications with the PC
  while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();			// Init SPI bus
  mfrc522.PCD_Init();		// Init MFRC522
}

void loop() {
  if (Serial.available())
  {
    Read();
    if (strcmp(serRead, "init") == 0)
    {
      Write("ok");
      Read();
      if (strcmp(serRead, "ok") == 0)
        auth = true;
      else
        auth = false;
    }
    else if (strcmp(serRead, "get") == 0)
    {
      if (auth && mfrc522.stringUid.length())
      {
        Write(mfrc522.stringUid);
      }
    }
    else if (strcmp(serRead, "auth") == 0)
    {
      Write(String(auth));
    }
    else if (strcmp(serRead, "reset") == 0)
    {
      auth = false;
      mfrc522.stringUid = "";
    }
    else if (strcmp(serRead, "check") == 0)
    {
      Write(String(checkTag()));
    }
  }
  
  if (checkTag())
    mfrc522.GetUid(&(mfrc522.uid), false);
}

void Write(String data)
{
  data += '\n';
  Serial.print(data);
}

void Read()
{
  for (int i = 0; i < count; i++)
    serRead[i] = 0;
    
  count = 0;
  
  char val;
  while (true)
  {
    if (Serial.available())
    {
      val = Serial.read();
      if (val > 0)
      {
        if (val == endTerm)
          break;
          
        serRead[count] = val;
        count++;
      }
    }
  }
}

bool checkTag()
{
  bool conn = false;
  if (mfrc522.PICC_IsNewCardPresent())
  {
    conn = mfrc522.PICC_ReadCardSerial();
    mfrc522.PICC_IsNewCardPresent();
  }
  return conn;
}
