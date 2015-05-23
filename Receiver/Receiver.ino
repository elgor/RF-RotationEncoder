/* 
If the Button is pressed a package is sent over the radio

 
 The circuit:
 * LED attached from pin 13 to ground
 
 * Hardware SPI:
 * MISO -> 12
 * MOSI -> 11
 * SCK -> 13

 * Configurable:
 * CE -> 8
 
*/


#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>


#define RADIO_PAYLOAD_TYPE int
#define RADIO_ADDRESS "addr1"

// constants
const int buttonPin = 2;     // the number of the pushbutton pin
const int ledPin =  5;      // the number of the LED pin
const int led2Pin =  6;      // the number of the LED pin
const int cePin =  9;      // chip enable
const int csnPin =  10;      // chip select

// Variables will change:
int ledState = HIGH;         // the current state of the output pin


void RF_setup(){
  Mirf.cePin = cePin;
  Mirf.csnPin = csnPin;

  Mirf.spi = &MirfHardwareSpi;  // get SPI driver
  Mirf.init();		// receive 1 byte
  
  Mirf.setRADDR( (byte *) RADIO_ADDRESS );    // Set Receiver Address
  Mirf.setTADDR( (byte *) RADIO_ADDRESS );    // Set Transmitter Address
  Mirf.payload = sizeof( RADIO_PAYLOAD_TYPE );
  Mirf.channel = 2;
  Mirf.config();
}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
 
  RF_setup();
}



void transmitPackage(RADIO_PAYLOAD_TYPE data) {
  Mirf.send((byte *)&data);
  digitalWrite(ledPin, HIGH);
  while(Mirf.isSending()){};
  delay(500);
  digitalWrite(ledPin, LOW);
}


RADIO_PAYLOAD_TYPE receivePackage(){
  RADIO_PAYLOAD_TYPE data = 0;
  int toggle = LOW;
  
  while(!Mirf.dataReady()){
    toggle ^= 1;
    digitalWrite(led2Pin, toggle);
    delay(500);  
  };
  Mirf.getData((byte *) &data);
  return data;
}


void loop() {
  RADIO_PAYLOAD_TYPE data = 0;
  
  // get data
  data = receivePackage();
  digitalWrite(ledPin, HIGH);
  
  // check data
  if(data == 22)
  {
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
  }
}
