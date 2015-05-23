/* 
If the Button is pressed a package is sent over the radio

 
 The circuit:
 * LED attached from pin 13 to ground
 * pushbutton attached from pin 2 to +5V
 * 10K resistor attached from pin 2 to ground
 
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
const int button1Pin = 2;     // the number of the pushbutton pin
const int button2Pin = 4;     // the number of the pushbutton pin
const int ledPin =  5;      // the number of the LED pin
const int cePin =  9;      // chip enable
const int csnPin =  10;      // chip select


// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 0;    // the debounce time; increase if the output flickers



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
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(ledPin, OUTPUT);
 
  RF_setup();
}



void transmitPackage(RADIO_PAYLOAD_TYPE data) {
  Mirf.send((byte *)&data);
  digitalWrite(ledPin, HIGH);
  while(Mirf.isSending()){};
  digitalWrite(ledPin, LOW);
}


RADIO_PAYLOAD_TYPE receivePackage(){
  RADIO_PAYLOAD_TYPE data = 0;
  while(!Mirf.dataReady()){};
  Mirf.getData((byte *) &data);
  return data;
}


int getButtonState(){
  int currentButtonState = digitalRead(button1Pin) | (digitalRead(button2Pin) << 1);
  int buttonState = lastButtonState;


  if (currentButtonState != lastButtonState) { 
    lastDebounceTime = millis();    // reset the debouncing timer
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    buttonState = currentButtonState;
  }


  lastButtonState = currentButtonState;
  return buttonState;
}


void loop() {
  
  buttonState = getButtonState();
  
  transmitPackage(buttonState);
  digitalWrite(ledPin, HIGH);
  
  digitalWrite(ledPin, (buttonState && int(1)));
  delayMicroseconds(10);
}
