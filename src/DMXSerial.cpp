// DMXSerial - A Arduino library for sending and receiving DMX using the builtin serial hardware port.
// DMXSerial.cpp: Library implementation file
//
// Copyright (c) 2011 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Documentation and samples are available at http://www.mathertel.de/Arduino
// 
// Heavily simplified by Otto Urpelainen. Anything not needed to send data using Atmega328P was removed.

#include "DMXSerial.h"
#include <avr/interrupt.h>

// ----- Constants -----

// formats for serial transmission
#define SERIAL_8N1  ((0<<USBS0) | (0<<UPM00) | (3<<UCSZ00))
#define SERIAL_8N2  ((1<<USBS0) | (0<<UPM00) | (3<<UCSZ00))
#define SERIAL_8E1  ((0<<USBS0) | (2<<UPM00) | (3<<UCSZ00))
#define SERIAL_8E2  ((1<<USBS0) | (2<<UPM00) | (3<<UCSZ00))

// the break timing is 10 bits (start + 8 data + parity) of this speed
// the mark-after-break is 1 bit of this speed plus approx 6 usec
// 100000 bit/sec is good: gives 100 usec break and 16 usec MAB
// 1990 spec says transmitter must send >= 92 usec break and >= 12 usec MAB
// receiver must accept 88 us break and 8 us MAB
#define BREAKSPEED     100000
#define DMXSPEED       250000
#define BREAKFORMAT    SERIAL_8E1
#define DMXFORMAT      SERIAL_8N2

// ----- Macros -----

// calculate prescaler from baud rate and cpu clock rate at compile time
// nb implements rounding of ((clock / 16) / baud) - 1 per atmega datasheet
#define Calcprescale(B)     ( ( (((F_CPU)/8)/(B)) - 1 ) / 2 )

// ----- DMXSerial Private variables -----
// These variables are not class members because they have to be reached by the interrupt implementations.
// don't use these variable from outside, use the appropriate methods.

int     _dmxChannel;  // the next channel byte to be sent.

volatile unsigned int  _dmxMaxChannel = 32; // the last channel used for sending (1..32).

// Array of DMX values (raw).
// Entry 0 will never be used for DMX data but will store the startbyte (0 for DMX mode).
uint8_t  _dmxData[DMXSERIAL_MAX+1];

// This pointer will point to the next byte in _dmxData;
uint8_t *_dmxDataPtr;

// This pointer will point to the last byte in _dmxData;
uint8_t *_dmxDataLastPtr;

// Create a single class instance. Multiple class instances (multiple simultaneous DMX ports) are not supported.
DMXSerialClass DMXSerial;


// ----- forwards -----

void _DMXSerialInit(uint16_t baud_setting, uint8_t mode, uint8_t format);
inline void _DMXSerialWriteByte(uint8_t data);

void _DMXStartSending();


// ----- Class implementation -----

// (Re)Initialize.
void DMXSerialClass::init()
{
  // initialize global variables
  _dmxChannel = 0;
  _dmxDataPtr = _dmxData;

  _dmxMaxChannel = DMXSERIAL_MAX; // The default in Receiver mode is reading all possible 512 channels.
  _dmxDataLastPtr = _dmxData + _dmxMaxChannel;

  // initialize the DMX buffer
  for (int n = 0; n < DMXSERIAL_MAX+1; n++) {
    _dmxData[n] = 0;
  }

  // now start
  _dmxMaxChannel = 32; // The default in Controller mode is sending 32 channels.
  _DMXStartSending();      
}


// Set the maximum used channel.
// This method can be called any time before or after the init() method.
void DMXSerialClass::maxChannel(int channel)
{
  if (channel < 1) channel = 1;
  if (channel > DMXSERIAL_MAX) channel = DMXSERIAL_MAX;

  _dmxMaxChannel = channel;
  _dmxDataLastPtr = _dmxData + channel;
}


// Read the current value of a channel.
uint8_t DMXSerialClass::read(int channel)
{
  // adjust parameter
  if (channel < 1) channel = 1;
  if (channel > DMXSERIAL_MAX) channel = DMXSERIAL_MAX;

  return(_dmxData[channel]);
}


// Write the value into the channel.
// The value is just stored in the sending buffer and will be picked up
// by the DMX sending interrupt routine.
void DMXSerialClass::write(int channel, uint8_t value)
{
  // adjust parameters
  if (channel < 1) channel = 1;
  if (channel > DMXSERIAL_MAX) channel = DMXSERIAL_MAX;
  if (value < 0)   value = 0;
  if (value > 255) value = 255;

  // store value for later sending
  _dmxData[channel] = value;

  // Make sure we transmit enough channels for the ones used
  if (channel > _dmxMaxChannel) {
    _dmxMaxChannel = channel;
    _dmxDataLastPtr = _dmxData + _dmxMaxChannel;
  }
}


// Return the DMX buffer of unsave direct but faster access 
uint8_t *DMXSerialClass::getBuffer()
{
  return(_dmxData);
}


// Terminate operation
void DMXSerialClass::term(void)
{
  // Disable all USART Features, including Interrupts
  UCSR0B = 0;
}


// ----- internal functions and interrupt implementations -----


// Initialize the Hardware serial port with the given baud rate
// using 8 data bits, no parity, 2 stop bits for data
// and 8 data bits, even parity, 1 stop bit for the break
void _DMXSerialInit(uint16_t baud_setting, uint8_t mode, uint8_t format)
{
  // assign the baud_setting to the USART Baud Rate Register
  UCSR0A = 0;                 // 04.06.2012: use normal speed operation
  UBRR0H = baud_setting >> 8;
  UBRR0L = baud_setting;

  // enable USART functions RX, TX, Interrupts
  UCSR0B = mode;
  
  // 2 stop bits and 8 bit character size, no parity
  UCSR0C = format;
}


// Setup Hardware for Sending
void _DMXStartSending()
{
  // Start sending a BREAK and send more bytes in UDRE ISR
  // Enable transmitter and interrupt
  _DMXSerialInit(Calcprescale(BREAKSPEED), ((1 << TXEN0) | (1 << TXCIE0)), BREAKFORMAT);

  _DMXSerialWriteByte((uint8_t)0);
}


// send the next byte after current byte was sent completely.
inline void _DMXSerialWriteByte(uint8_t data)
{
  // putting data into buffer sends the data
  UDR0 = data;
}


// Interrupt service routines that are called when the actual byte was sent.
// When changing speed (for sending break and sending start code) we use TX finished interrupt
// which occurs shortly after the last stop bit is sent
// When staying at the same speed (sending data bytes) we use data register empty interrupt
// which occurs shortly after the start bit of the *previous* byte
// When sending a DMX sequence it just takes the next channel byte and sends it out.
// In DMXController mode when the buffer was sent completely the DMX sequence will resent, starting with a BREAK pattern.
ISR(USART_TX_vect)
{
  if (_dmxChannel == -1) {
    // this interrupt occurs after the stop bits of the last data byte
    // start sending a BREAK and loop forever in ISR
    _DMXSerialInit(Calcprescale(BREAKSPEED), ((1 << TXEN0) | (1 << TXCIE0)), BREAKFORMAT);
    _DMXSerialWriteByte((uint8_t)0);
    _dmxChannel = 0;

  } else if (_dmxChannel == 0) {
    // this interrupt occurs after the stop bits of the break byte

    // now back to DMX speed: 250000baud
    // take next interrupt when data register empty (early)
    _DMXSerialInit(Calcprescale(DMXSPEED), ((1 << TXEN0) | (1 << UDRIE0)), DMXFORMAT);

    // write start code
    _DMXSerialWriteByte((uint8_t)0);
    _dmxChannel = 1;
  }
}


  // this interrupt occurs after the start bit of the previous data byte
ISR(USART_UDRE_vect)
{
  _DMXSerialWriteByte(_dmxData[_dmxChannel++]);

  if (_dmxChannel > _dmxMaxChannel) {
    _dmxChannel = -1; // this series is done. Next time: restart with break.
    // get interrupt after this byte is actually transmitted
    // UCSRnB = (1 << TXENn) | (1 << TXCIEn);
    _DMXSerialInit(Calcprescale(DMXSPEED), ((1 << TXEN0) | (1 << TXCIE0)), DMXFORMAT);
  } // if

} // ISR(USARTn_UDRE_vect)

// The End
