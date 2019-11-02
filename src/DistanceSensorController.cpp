#include "config.h"

#include "AvrUtils.h"

#include "DistanceSensorController.h"

#include <util/delay.h>
#include <avr/interrupt.h>

// Ultrasound travel delay
uint32_t counter = 0;
uint32_t delay = 0;

Port echoPortStatic;
uint8_t echoPinStatic;

DistanceSensorController::DistanceSensorController(
    Port triggerPort,
    uint8_t triggerPin,
    Port echoPort,
    uint8_t echoPin
) :
    triggerPort(triggerPort),
    triggerPin(triggerPin),
    echoPort(echoPort),
    echoPin(echoPin),
    triggerState(false) {
    // Interrupt need to know the echo pin, too
    echoPortStatic = echoPort;
    echoPinStatic = echoPin;

    setDataDirection(triggerPort, triggerPin, true);    
    setDataDirection(echoPort, echoPin, false, false);    

    setData(triggerPort, triggerPin, false);

    enablePinChangeInterrupt(echoPort, echoPin);

    initializeTimer0(PSV_1, NORMAL, TOP_00FF);
    // Enable interrupt on overflow
    TIMSK0 |= BV(TOIE0);
}

float DistanceSensorController::run() {
    triggerState = !triggerState;
    setData(triggerPort, triggerPin, triggerState);

    return 0.3840*delay;
}

// TODO: This works only if echo is connected to Port C. Should support also
// other pins.
ISR(PCINT1_vect) {
    bool isHigh = getData(echoPortStatic, echoPinStatic);
    if (isHigh) {
        // Start of measurement, reset values
        counter = 0;
    }
    else {
        // Measurement done, save measured delay
        delay = counter;
    }
}

ISR(TIMER0_OVF_vect) {
    counter++;
}