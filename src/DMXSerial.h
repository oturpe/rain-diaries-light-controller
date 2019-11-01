// DMXSerial - A Arduino library for sending DMX using the builtin serial hardware port.
// DMXSerial.h: Library header file
// 
// Copyright (c) 2011-2014 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
// 
// Heavily simplified by Otto Urpelainen. Anything not needed to send data using Atmega328P was removed.

#ifndef DmxSerial_h
#define DmxSerial_h

#include "config.h"

#include <avr/io.h>

#define DMXSERIAL_MAX 512 ///< max. number of supported DMX data channels

// ----- Library Class -----

extern "C" {
  typedef void (*dmxUpdateFunction)(void);
}

/**
 * @brief Arduino library to send DMX.
 */
class DMXSerialClass
{
  public:
    /**
     * @brief Initialize or re-initialize
     * @return void    
     */
    void init();


    /**
     * @brief Set the maximum used channel.
     * @param [in] channel The highest channel that will be transferred. 
     * @return void    
     */
    void maxChannel(int channel);

    /**
     * @brief Read the current value of a channel.
     * @param [in] channel The channel number.
     * @return uint8_t The current value.
     */
    uint8_t read(int channel);

    /**
     * @brief Write a new value to a channel.
     * @param [in] channel The channel number.
     * @param [in] value The current value.
     * @return void
     */
    void write(int channel, uint8_t value);

    /**
     * @brief Get a pointer to DMX Buffer.
     * This is the internal byte-array where the current DMX values are stored. 
     * @return uint8_t DMX values buffer.
     */
    uint8_t *getBuffer();
    
    /**
     * @brief Terminate the current operation mode.
     */
    void    term();
};

// Use the DMXSerial library through the DMXSerial object.
// There is only one DMX port supported and DMXSerial is a static object.
extern DMXSerialClass DMXSerial;

#endif
