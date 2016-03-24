#include <SPI.h>

const uint16_t k_uAConfigBits = 0x7000;
const uint16_t k_uBConfigBits = 0xf000;
const int chipASelectPin = 47;
//const int chipBSelectPin = 49; // This pin is not working. Just stays high.

void setup() {
  // put your setup code here, to run once:

  pinMode(chipASelectPin, OUTPUT);
  digitalWrite(chipASelectPin, HIGH);

  // Use the slowest
  SPI.beginTransaction(SPISettings(1525000, MSBFIRST, SPI_MODE0));
  SPI.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

  noInterrupts();
  //digitalWrite(chipASelectPin, LOW);
  for(uint16_t uIndex = 0; uIndex < 4096; ++uIndex)
  {
    digitalWrite(chipASelectPin, LOW);
    SPI.transfer16(k_uAConfigBits | uIndex);
    digitalWrite(chipASelectPin, HIGH);
    digitalWrite(chipASelectPin, LOW);
    SPI.transfer16(k_uBConfigBits | (4095 - uIndex));
    digitalWrite(chipASelectPin, HIGH);
    //SetDac(uIndex, 0);
  }

  for(uint16_t uIndex = 0; uIndex < 4096; ++uIndex)
  {
    digitalWrite(chipASelectPin, LOW);
    SPI.transfer16(k_uAConfigBits | (4095 - uIndex));
    digitalWrite(chipASelectPin, HIGH);
    digitalWrite(chipASelectPin, LOW);
    SPI.transfer16(k_uBConfigBits | uIndex);
    digitalWrite(chipASelectPin, HIGH);
    //SetDac((4095 - uIndex), 0);
  }
  //digitalWrite(chipASelectPin, HIGH);
  interrupts();
}

void SetDac(int iValue, int iChannel)
{
  byte byDacRegister = 0b01110000;
  int iSecondaryByteMask = 0b0000000011111111;
  byte byDacPrimaryByte = (iValue >> 8) | byDacRegister;
  byte byDacSecondaryByte = iValue & iSecondaryByteMask;
  switch(iChannel)
  {
    case 0:
      byDacPrimaryByte &= ~(1 << 7);
      break;
    case 1:
      byDacPrimaryByte |= (1 << 7);
      break;
  }
  noInterrupts();
  digitalWrite(chipASelectPin, LOW);
  SPI.transfer(byDacPrimaryByte);
  SPI.transfer(byDacSecondaryByte);
  digitalWrite(chipASelectPin, HIGH);
  interrupts();
}

