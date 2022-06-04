#include <hardware/pwm.h>

#pragma once

#define PWMSERVO_NEUTRAL 1500
#define PWMSERVO_MIN 1000
#define PWMSERVO_MAX 2000

#define _deg_2_micros_slope (PWMSERVO_MAX - PWMSERVO_MIN)/(180)

uint_fast16_t _clamp_val(uint_fast16_t val, uint_fast16_t min, uint_fast16_t max)
{
    const uint_fast16_t n = val < min ? min : val;
    return n > max ? max : n;
};

bool pwmservo_init(pin_size_t pin)
{
    // get slice number
    uint_fast8_t slicenum = pwm_gpio_to_slice_num(pin);
    
    // assign GPIO to pwm functionality
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // set the clkdiv mode (this might not actually do anything)
    pwm_set_clkdiv_mode(slicenum, PWM_DIV_FREE_RUNNING);

    // disable phase correct (if enabled, frequency is halved and duty cycle is doubled)
    pwm_set_phase_correct(slicenum, false);

    // set clkdiv to system clock in Mhz (125Mhz clock = clkdiv of 125)
    // makes it so that our on/off threshold on each pwm channel is equal to the on time per cycle in microseconds
    pwm_set_clkdiv_int_frac(slicenum, clock_get_hz(clk_sys)/1000000, 0);

    // set wrap to 20000 to get a duty cycle of 50hz for standard servos
    pwm_set_wrap(slicenum, 20000);

    // enable PWM slice
    pwm_set_enabled(slicenum, true);

    // enable PWM pin
    pwm_set_gpio_level(pin, PWMSERVO_NEUTRAL);

    return true;
};

void pwmservo_write_micros(pin_size_t pin, uint_fast16_t val)
{
    pwm_set_gpio_level(pin, _clamp_val(val, PWMSERVO_MIN, PWMSERVO_MAX));
};

void pwmservo_write(pin_size_t pin, uint_fast16_t angle)
{
    pwm_set_gpio_level(pin, (_deg_2_micros_slope*_clamp_val(angle, 0, 180)) + PWMSERVO_MIN);
};
