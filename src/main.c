#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/devicetree.h>

LOG_MODULE_REGISTER(buzzer_app, LOG_LEVEL_DBG);

/* Musical note frequencies (Hz) */
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_PAUSE 0  // Pause/rest

/* Note durations */
#define WHOLE_NOTE     1600
#define HALF_NOTE      800
#define QUARTER_NOTE   400
#define EIGHTH_NOTE    200
#define SIXTEENTH_NOTE 100

/* Buzzer node defined in the overlay */
static const struct pwm_dt_spec buzzer_pwm =
    PWM_DT_SPEC_GET(DT_PATH(zephyr_user));

/* LED GPIO pin 2.00 (port 2, pin 0) */
#define LED_PORT_NUM 2
#define LED_PIN_NUM 0
static const struct device *led_gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio2));

/* We Wish You a Merry Christmas melody */
/* Based on Arduino Christmas Songs by Dipto Pratyaksa, modified by Joshi */
static const uint16_t merry_christmas_notes[] = {
    NOTE_B3,
    NOTE_F4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_E4,
    NOTE_D4, NOTE_D4, NOTE_D4,
    NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_F4,
    NOTE_E4, NOTE_E4, NOTE_E4,
    NOTE_A4, NOTE_A4, NOTE_B4, NOTE_A4, NOTE_G4,
    NOTE_F4, NOTE_D4, NOTE_B3, NOTE_B3,
    NOTE_D4, NOTE_G4, NOTE_E4,
    NOTE_F4
};

static const uint16_t merry_christmas_durations[] = {
    QUARTER_NOTE,        /* 4 */
    QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,  /* 4, 8, 8, 8, 8 */
    QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,  /* 4, 4, 4 */
    QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,  /* 4, 8, 8, 8, 8 */
    QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,  /* 4, 4, 4 */
    QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,  /* 4, 8, 8, 8, 8 */
    QUARTER_NOTE, QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,  /* 4, 4, 8, 8 */
    QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,  /* 4, 4, 4 */
    HALF_NOTE  /* 2 */
};

#define MELODY_SIZE (sizeof(merry_christmas_notes) / sizeof(merry_christmas_notes[0]))

/* LED control functions */
static void led_on(void)
{
    if (device_is_ready(led_gpio_dev)) {
        gpio_pin_set(led_gpio_dev, LED_PIN_NUM, 1);
    }
}

static void led_off(void)
{
    if (device_is_ready(led_gpio_dev)) {
        gpio_pin_set(led_gpio_dev, LED_PIN_NUM, 0);
    }
}

/* Play a single note */
static void play_note(uint16_t frequency, uint16_t duration_ms)
{
    if (frequency == NOTE_PAUSE) {
        /* Silence - turn LED off */
        led_off();
        pwm_set_dt(&buzzer_pwm, 0, 0);
        k_msleep(duration_ms);
        return;
    }
    
    /* Turn LED on when playing note */
    led_on();
    
    /* Calculate period in nanoseconds */
    uint32_t period_ns = (NSEC_PER_SEC / frequency);
    /* Use 90% duty cycle for maximum volume (passive buzzers respond better to higher duty) */
    uint32_t pulse_ns = (period_ns * 9) / 10;  /* 90% duty cycle for louder sound */
    
    /* Start PWM */
    int ret = pwm_set_dt(&buzzer_pwm, period_ns, pulse_ns);
    if (ret != 0) {
        LOG_ERR("Failed to set PWM for note %d Hz: %d", frequency, ret);
    }
    
    /* Play note for duration */
    k_msleep(duration_ms);
    
    /* Stop PWM (silence) and turn LED off */
    pwm_set_dt(&buzzer_pwm, period_ns, 0);
    led_off();
    k_msleep(30);  /* Smaller pause between notes */
}

/* Play melody */
static void play_melody(void)
{
    LOG_INF("Playing We Wish You a Merry Christmas melody...");
    
    for (size_t i = 0; i < MELODY_SIZE; i++) {
        play_note(merry_christmas_notes[i], merry_christmas_durations[i]);
    }
    
    LOG_INF("Melody finished");
}

/* Simple beep function with configurable duty cycle */
static void simple_beep_with_duty(uint8_t duty_percent)
{
    LOG_DBG("Simple beep at %d%% duty", duty_percent);
    
    led_on();
    
    uint32_t period_ns = NSEC_PER_SEC / 4000;  /* 4 kHz = 250000 ns period */
    uint32_t pulse_ns = (period_ns * duty_percent) / 100;
    
    pwm_set_dt(&buzzer_pwm, period_ns, pulse_ns);
    k_msleep(500);
    pwm_set_dt(&buzzer_pwm, period_ns, 0);
    
    led_off();
}

/* Simple beep function */
static void simple_beep(void)
{
    /* Try 90% duty cycle for maximum volume */
    simple_beep_with_duty(90);
}

int main(void)
{
    LOG_INF("=== Buzzer nRF54 Application ===");
    
    if (!device_is_ready(buzzer_pwm.dev)) {
        LOG_ERR("PWM device not ready: %s", buzzer_pwm.dev->name);
        return 0;
    }
    
    LOG_INF("PWM device ready: %s", buzzer_pwm.dev->name);
    LOG_INF("PWM channel: %d", buzzer_pwm.channel);
    
    /* Initialize LED GPIO pin 2.00 */
    if (!device_is_ready(led_gpio_dev)) {
        LOG_ERR("GPIO device not ready: %s", led_gpio_dev->name);
        return 0;
    }
    
    int ret = gpio_pin_configure(led_gpio_dev, LED_PIN_NUM, GPIO_OUTPUT_INACTIVE);
    if (ret != 0) {
        LOG_ERR("Failed to configure LED GPIO pin: %d", ret);
        return 0;
    }
    
    LOG_INF("LED GPIO pin 2.%02d configured", LED_PIN_NUM);
    
    /* Wait a bit for system to stabilize */
    k_msleep(1000);
    
    while (1) {
        /* Play melody */
        play_melody();
        
        /* Wait 2 seconds */
        k_msleep(2000);
        
        /* Play simple beep */
        simple_beep();
        
        /* Wait 5 seconds before next cycle */
        k_msleep(5000);
    }
    
    return 0;
}

