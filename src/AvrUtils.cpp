#include <avr/io.h>
#include <avr/interrupt.h>

#include "AvrUtils.h"

bool getData(Port port, int pin) {
  switch (port) {
    case B:
        return PINB & BV(pin);
    case C:
        return PINC & BV(pin);
    case D:
        return PIND & BV(pin);
    default:
         // Should not happen
         return false;
  }
}

void setData(Port port, int pin, bool enable) {
    if (enable) {
        // Enable pin
        switch (port) {
            case B:
                PORTB |= BV(pin);
                return;
            case C:
                PORTC |= BV(pin);
                return;
            case D:
                PORTD |= BV(pin);
                return;
            default:
                // Should not happen
                return;
        }
    }
    else {
        // Disable pin
        switch (port) {
            case B:
                PORTB &= ~BV(pin);
                return;
            case C:
                PORTC &= ~BV(pin);
                return;
            case D:
                PORTD &= ~BV(pin);
                return;
            default:
                // Should not happen
                return;
        }
    }
}

void setDataDirection(Port port, int pin, bool enable) {
    if (enable) {
        // Enable pin
        switch (port) {
            case B:
                DDRB |= BV(pin);
                return;
            case C:
                DDRC |= BV(pin);
                return;
            case D:
                DDRD |= BV(pin);
                return;
            default:
                // Should not happen
                return;
        }
    }
    else {
        // Disable pin
        switch (port) {
            case B:
                DDRB &= ~BV(pin);
                return;
            case C:
                DDRC &= ~BV(pin);
                return;
            case D:
                DDRD &= ~BV(pin);
                return;
            default:
                // Should not happen
                return;
        }
    }
}

void enablePinChangeInterrupt(Port port, int pin) {
    switch (port) {
        case B:
            // Pins in port B are mapped to Pin Change Interrupt 0
            PCICR |= (1 << PCIE0);
            switch (pin) {
                case 0:
                    PCMSK0 |= (1 << PCINT0);
                    break;
                case 1:
                    PCMSK0 |= (1 << PCINT1);
                    break;
                case 2:
                    PCMSK0 |= (1 << PCINT2);
                    break;
                case 3:
                    PCMSK0 |= (1 << PCINT3);
                    break;
                case 4:
                    PCMSK0 |= (1 << PCINT4);
                    break;
                case 5:
                    PCMSK0 |= (1 << PCINT5);
                    break;
                case 6:
                    PCMSK0 |= (1 << PCINT6);
                    break;
                case 7:
                    PCMSK0 |= (1 << PCINT7);
                    break;
                default:
                    // Should not happen
                    break;
            }
            return;
        case C:
            // Pins in port C are mapped to Pin Change Interrupt 1
            PCICR |= (1 << PCIE1);
            switch (pin) {
                case 0:
                    PCMSK1 |= (1 << PCINT8);
                    break;
                case 1:
                    PCMSK1 |= (1 << PCINT9);
                    break;
                case 2:
                    PCMSK1 |= (1 << PCINT10);
                    break;
                case 3:
                    PCMSK1 |= (1 << PCINT11);
                    break;
                case 4:
                    PCMSK1 |= (1 << PCINT12);
                    break;
                case 5:
                    PCMSK1 |= (1 << PCINT13);
                    break;
                case 6:
                    PCMSK1 |= (1 << PCINT14);
                    break;
                case 7:
                    // Not defined in avr headers, apparently does not exist
                    //PCMSK1 |= (1 << PCINT15);
                    break;
                default:
                    // Should not happen
                    break;
            }
            return;
        case D:
            // Pins in port D are mapped to Pin Change Interrupt 2
            PCICR |= (1 << PCIE2);
            switch (pin) {
                case 0:
                    PCMSK2 |= (1 << PCINT16);
                    break;
                case 1:
                    PCMSK2 |= (1 << PCINT17);
                    break;
                case 2:
                    PCMSK2 |= (1 << PCINT18);
                    break;
                case 3:
                    PCMSK2 |= (1 << PCINT19);
                    break;
                case 4:
                    PCMSK2 |= (1 << PCINT20);
                    break;
                case 5:
                    PCMSK2 |= (1 << PCINT21);
                    break;
                case 6:
                    PCMSK2 |= (1 << PCINT22);
                    break;
                case 7:
                    PCMSK2 |= (1 << PCINT23);
                    break;
                default:
                    // Should not happen
                    break;
            }
            return;
        default:
            // Should not happen
            return;
    }
}

void initializeTimer0(
    TimerPrescalerValue prescalerValue,
    WaveformGenerationMode mode,
    CounterTop top
) {
    switch (prescalerValue) {
    case PSV_1:
        TCCR0B |= BV(CS00);
        TCCR0B &= ~BV(CS02) & ~BV(CS01);
        break;
    case PSV_8:
        TCCR0B |= BV(CS01);
        TCCR0B &= ~BV(CS02) & ~BV(CS00);
        break;
    case PSV_64:
        TCCR0B |= BV(CS01) | BV(CS00);
        TCCR0B &= ~BV(CS02);
        break;
    case PSV_256:
        TCCR0B |= BV(CS02);
        TCCR0B &= ~BV(CS01) & ~BV(CS00);
        break;
    case PSV_1024:
        TCCR0B |= BV(CS02) | BV(CS00);
        TCCR0B &= ~BV(CS01);
        break;
    }

    switch (mode) {
    case NORMAL:
        TCCR0A &= ~BV(WGM01) & ~BV(WGM00);
        TCCR0B &= ~BV(WGM02);
        break;
    case PWM_PHASE_CORRECT:
        TCCR0A |= BV(WGM00);
        TCCR0A &= ~BV(WGM01);
        if (top == TOP_OCRA) {
            TCCR0B |= BV(WGM02);
        } else {
            TCCR0B &= ~BV(WGM02);
        }
        break;
    case PWM_FAST:
        TCCR0A |= BV(WGM01) | BV(WGM00);
        TCCR0A &= ~BV(WGM02);
        if (top == TOP_OCRA) {
            TCCR0B |= BV(WGM02);
        } else {
            TCCR0B &= ~BV(WGM02);
        }
        break;
    case CTC:
        TCCR0A |= BV(WGM01);
        TCCR0A &= ~BV(WGM00);
        TCCR0B &= ~BV(WGM02);
        break;
    }
}
