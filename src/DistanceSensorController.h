#ifndef _H_DISTANCE_SENSOR_CONTROLLER
#define _H_DISTANCE_SENSOR_CONTROLLER

#include "AvrUtils.h"

#include <stdint.h>

/// \class DistanceSensorController
///
/// Operates HC-SR04 ultrasound distance sensor.
class DistanceSensorController {
public:
    /// \brief
    ///    Initializes a new controller instance.
    ///
    /// \param triggerPort
    ///     Port where sensor trigger is connected
    /// \param triggerPin
    ///     Pin where sensor trigger is connected
    /// \param echoPort
    ///     Port where sensor echo is connected
    /// \param echoPin
    ///     Pin where sensor echo is connected
    DistanceSensorController(
        Port triggerPort,
        uint8_t triggerPin,
        Port echoPort,
        uint8_t echoPin
    );

public:
    /// \brief
    ///    Instructs the controller to advance one step in sequence, essentially
    ///    stepping the controller's clock.
    ///
    /// \return
    ///     Distance of target in units of centimeter if new reception is
    ///     available, otherwise 0.
    float run();

private:
    /// Port where sensor trigger is connected.
    const Port triggerPort;
    /// Pin where sensor trigger is connected.
    const uint8_t triggerPin;
    /// Port where sensor echo is connected.
    const Port echoPort;
    /// Pin where sensor echo is connected.
    const uint8_t echoPin;

    /// Trigger state. This is switched back and forth between run() calls.
    bool triggerState;
};

#endif
