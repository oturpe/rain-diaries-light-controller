#include <stdint.h>
#include <stdlib.h>

#include "SingleChannelFlickeringDmxController.h"

#include "DMXSerial.h"

SingleChannelFlickeringDmxController::SingleChannelFlickeringDmxController(
    uint16_t channel,
    uint8_t baseline,
    uint8_t flicker
) : channel(channel), baseline(baseline), flicker(flicker) {
    DMXSerial.init();
}

void SingleChannelFlickeringDmxController::run() {
    counter++;

    uint16_t brightness = baseline + (rand() % flicker);
    if (brightness > 255) {
        brightness = 255;
    }

    DMXSerial.write(channel, brightness);
}
