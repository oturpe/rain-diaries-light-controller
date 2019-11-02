// Reads a sensor and outputs a flicker on DMX bus.
//
// Author: Otto Urpelainen
// Email: oturpe@iki.fi
// Date: 2019-10-31

#include "AvrUtils.h"

#include "config.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#include "IndicatorController.h"
#include "SingleChannelFlickeringDmxController.h"
#include "DistanceSensorController.h"

// Temp, needed for testing only
#include "DMXSerial.h"

int main() {
    IndicatorController indicator(C, 0, 20);
    DistanceSensorController distanceSensorController(C, 2, C, 3);
    SingleChannelFlickeringDmxController dmx(
        1,
        LIGHT_BRIGHTNESS_BASELINE,
        LIGHT_FLICKER_INTENSITY
    );

    sei();

    // Temp, to be able to test with the particular multichannel dmx light used
    // in testing.
    DMXSerial.write(1/*channel*/ + 4, 255);

    while (true) {
         indicator.run();
         float distance = distanceSensorController.run();
         dmx.setFlickerEnabled(distance < DISTANCE_THRESHOLD);
         dmx.run();
        _delay_ms(LOOP_DELAY);
    }
}
