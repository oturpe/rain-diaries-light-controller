// This is the configuration file. Ideally, any configuration and calibration
// of the device should be done by changing this file only.

// Needed by util/delay.h
#define F_CPU 16000000UL

// Delay between two executions of main loop, given in millisecond.
#define LOOP_DELAY 25

// Baseline brightness of the light. Given as value between 0 and 255.
#define LIGHT_BRIGHTNESS_BASELINE 64
// Flicker intensity of the light. Given in same units as
// LIGHT_BRIGHTNESS_BASELINE.
#define LIGHT_FLICKER_INTENSITY 20
// Distance treshold for starting the flicker. Given in units of centimeter.
#define DISTANCE_THRESHOLD 200