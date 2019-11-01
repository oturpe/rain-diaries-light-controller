#ifndef _H_SINGLE_CHANNEL_FLICKERING_DMX_CONTROLLER
#define _H_SINGLE_CHANNEL_FLICKERING_DMX_CONTROLLER

/// \class SingleChannelFlickeringDmxController
///
/// Transmits a basically constant but flickering sequence in a single dmx
/// channel. Uses the Atmega328p serial interface.
class SingleChannelFlickeringDmxController {
public:
    /// \brief
    ///    Initializes a new controller instance.
    ///
    /// \param channel
    ///     Dmx channel number
    /// \param baseline
    ///     Baseline brightness around which the flicker happens
    /// \param flicker
    ///     Flicker intensity
    SingleChannelFlickeringDmxController(
        uint16_t channel,
        uint8_t baseline,
        uint8_t flicker
    );

public:
    /// \brief
    ///    Instructs the controller to advance one step in sequence, essentially
    ///    stepping the controller's clock.
    void run();

private:
    /// Dmx channel number
    const uint16_t channel;
    /// Baseline brightness
    const uint8_t baseline;
    /// Flicker intensity
    const uint8_t flicker;

    /// Current value
    uint32_t counter;
};

#endif
