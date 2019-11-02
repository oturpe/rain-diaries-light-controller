#include <stdint.h>
#include <stdlib.h>

#include "SingleChannelFlickeringDmxController.h"

#include "DMXSerial.h"

SingleChannelFlickeringDmxController::SingleChannelFlickeringDmxController(
    uint16_t channel,
    uint8_t baseline,
    uint8_t flicker
) :
    channel(channel),
    baseline(baseline),
    flicker(flicker),
    isFlickerEnabled(false) {
    // Further initialization
    DMXSerial.init();
}

void SingleChannelFlickeringDmxController::setFlickerEnabled(bool isFlickerEnabled) {
    this->isFlickerEnabled = isFlickerEnabled;
}

void SingleChannelFlickeringDmxController::run() {
    counter++;

    int16_t brightness = baseline;

    if (isFlickerEnabled) {
        brightness -= flicker/2;
        brightness += rand() % flicker;
    }

    if (brightness < 0) {
        brightness = 0;
    }
    if (brightness > 255) {
        brightness = 255;
    }

    DMXSerial.write(channel, brightness);
}
