#include <SPI.h>
// Ctrl + T for autoformat
#define CS 0              // Chip select
// AND W ADDR
#define WRITEBYTE 0x00
#define READBYTE 0x80
// Set 2nd MSB for Multi-Byte mode

// figure out speed at which sensor updates and samples
// and speed at which we poll for those
// as well as the SPI transfer speeds

// ADXL(R/W, REG, DATA)
// return 1 for write, could even do a read to check successful write
// ADXL.read

void initialise()
{
  Serial.begin(115200);
  SPI.begin();
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  while (!Serial);
  Serial.println("Initialized");
}

void registerWrite(byte Address, byte data)  {
  SPI.beginTransaction(SPISettings(5000000, MSBFIRST, SPI_MODE3));
  digitalWrite(CS, LOW);
  SPI.transfer(Address | WRITEBYTE);
  SPI.transfer(data);
  digitalWrite(CS, HIGH);
  SPI.endTransaction();
}

uint8_t registerRead(byte Address) {
  SPI.beginTransaction(SPISettings(5000000, MSBFIRST, SPI_MODE3));
  digitalWrite(CS, LOW);
  SPI.transfer(Address | READBYTE);
  uint8_t data = SPI.transfer(0x00);
  digitalWrite(CS, HIGH);
  SPI.endTransaction();
  return data;
}

void setup() {
  initialise();
  // Configure chip
  registerWrite(0x31, 0x00);            // Enter 4-wire SPI mode
  Serial.print("Reg0x31: "); Serial.println(registerRead(0x31));
  // Read & print DEVICE ID
  Serial.print("DEVICE ID: "); Serial.println(registerRead(0x00));
  // SET RES AND G-RANGE HERE
  registerWrite(0x2D, 0x08);            // Exit standby mode
}

float acceleration(char axis) {
  // Pass axis to registerRead (ASCII with offset)
  // Either do a switch or make sure user cant access
  // Out of bounds
  
  // Try to only use one variable<-- change int16 to float?
  
  int16_t DATAn = (registerRead(axis-(189-axis)) << 8);   // Hi
  DATAn |= registerRead(axis-(190-axis));                 // Lo
  // Divide value by the LSB/g(256 for 2g at 10-bit res)
  float nAccel = DATAn / 256.0;
  // Then times by Earth accel due to gravity (9.807m/s^2)
  nAccel *= 9.807;
  return nAccel;
}

void loop() {
  Serial.print("z: "); Serial.print(acceleration('z')); Serial.println(" m/s^2  ");
  //Serial.print("y: "); Serial.print(acceleration('y')); Serial.print(" m/s^2  ");
  //Serial.print("z: "); Serial.print(acceleration('z')); Serial.println(" m/s^2");
  delay(500);
}

// Able to change data update speed, G-range, resolution etc settings
// Add way to calibrate/offset

// IF 8 bit storage is recycled after the hi & lo byte are combined:
// Read xData store high and low byte
// Read yData store high and low byte
// Read zData store high and low byte

// Combine high and low byte 

// Convert to m/s^2
// ========================================

// Either do all axis at one time using a for loop OR
// one at a time 

// There are a few combinations of when registers should 
// Be read and when the numbers should be processed 
// The reason is to avoid other axis' registers changing value
// While processing/reading the current axis

// ADXL.read(x);
// ADXL.settings();

// Check if ADXL is ther by reading ID

// Shouldnt max accel be 1g? 
