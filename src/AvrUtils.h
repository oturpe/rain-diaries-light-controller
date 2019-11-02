// Generic Avr helpers

#ifndef _H_OTURPE_AVR_UTILS
#define _H_OTURPE_AVR_UTILS

// Cleaner setting of bits
#define BV(x) (1<<x)

/// \enum Port
///
/// Symbols for referring to separate io ports.
enum Port { B, C, D };

/// Possible prescaler values for timers.
enum TimerPrescalerValue {
    PSV_1,
    PSV_8,
    PSV_32,
    PSV_64,
    PSV_128,
    PSV_256,
    PSV_1024,
};

/// \enum WaveformGenerationMode
///    Possible waveform generation modes
enum WaveformGenerationMode {
    NORMAL,
    PWM_PHASE_CORRECT,
    PWM_PHASE_AND_FREQUENCY_CORRECT, // timer 1 only
    PWM_FAST,
    CTC
};

/// \enum CounterTop
///    Top value for counter
enum CounterTop {
    TOP_00FF,
    TOP_01FF, // timer 1 only
    TOP_02FF, // timer 1 only
    TOP_ICR, // timer 1 only
    TOP_OCRA
};

/// \brief
///    Read pin value.
///
/// \param port
///    Pin port
///
/// \param pin
///    Pin index
///
/// \return
///    If pin is high.
bool getData(Port port, int pin);

/// \brief
///    Sets value of a single bith in a data register (PORTxn). Normally used
///    to set output pin high or low.
///    
/// \param port
///    Pin port
///    
/// \param pin
///    Pin index
///
/// \param enable
///    If pin is enabled (high). Otherwise it is disabled (low).
void setData(Port port, int pin, bool enable);

/// \brief
///    Sets value of a single bith in a data direction register (PORTxn).
///    Used to set a pin as input or output.
///    
/// \param port
///    Pin port
///    
/// \param pin
///    Pin index
///
/// \param enable
///    If pin is enabled (output) or disabled (input)
///
/// \param enablePullup
///    If pullup resistor is enabled. This value is only used when configuring
///    an input pin.
void setDataDirection(Port port, int pin, bool enable, bool enablePullup = true);

/// \brief
///    Enables pin change intterupt on the given pin.
///
/// \param port
///    Pin port
///
/// \param pin
///    Pin index
void enablePinChangeInterrupt(Port port, int pin);

/// Initializes timer 0 by setting waveform generation mode and prescaler.
///
/// This function assumes that neither Clock Select nor Waveform Generation bits
/// have not been touched yet. Note that timer 0 does not support all
/// Timer0PrescalerValue values. If illegal value is entered, prescaler is not
/// set to any value.
///
/// Parameter top toggles using OCRA register values as the TOP for pulse
/// width modulation. It it only used if mode is either PWM_PHASE_CORRECT or
/// PWM_FAST.
///
/// \param prescalerValue
///    Requested prescaler value
///
/// \param mode
///    Waveform generation mode
///
/// \param top
///    Selection of counter TOP
void initializeTimer0(
    TimerPrescalerValue prescalerValue,
    WaveformGenerationMode mode,
    CounterTop top
);

#endif //_H_OTURPE_AVR_UTILS
