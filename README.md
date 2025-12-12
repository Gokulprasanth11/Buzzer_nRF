# Buzzer nRF54 - Musical Buzzer with LED Synchronization

A Zephyr RTOS application for the nRF54 development kit that plays musical melodies through a buzzer with synchronized LED blinking.

## 🎵 Features

- **Musical Melody Playback**: Plays the "Jingle Bells" melody using PWM-controlled buzzer
- **LED Synchronization**: LED at pin 2.00 blinks in sync with the music
- **Multiple Note Support**: Full range of musical notes from B0 to C8
- **Configurable Beeps**: Simple beep function with adjustable duty cycle
- **PWM Control**: Uses PWM20 for precise frequency control

## 🎹 Musical Capabilities

The application supports a full range of musical notes:
- **Octaves**: B0 through C8 (8+ octaves)
- **Note Types**: Whole, half, quarter, eighth, and sixteenth notes
- **Pauses**: Support for musical rests/pauses
- **Volume Control**: 90% duty cycle for maximum buzzer volume

## 🔌 Hardware Requirements

- **Board**: nRF54L15 Development Kit
- **Buzzer**: Passive buzzer connected to PWM pin
- **LED**: LED connected to GPIO pin 2.00 (port 2, pin 0)
- **PWM Pin**: P1.08 (configured as PWM20 channel 0)

## 📋 Pin Configuration

| Component | Pin | Function |
|-----------|-----|----------|
| Buzzer    | P1.08 | PWM20_OUT0 |
| LED       | P2.00 | GPIO Output |

## 🛠️ Building and Running

### Prerequisites

- Zephyr RTOS SDK installed
- nRF Connect SDK (or compatible Zephyr distribution)
- CMake 3.21.0 or higher
- Toolchain for ARM Cortex-M

### Build Instructions

```bash
# Navigate to project directory
cd buzzer_nrf54

# Build the project
west build -b nrf54l15dk/nrf54l15/cpuapp

# Flash to device
west flash
```

### Configuration

The project uses the following Zephyr configuration:
- `CONFIG_PWM=y` - Enable PWM driver
- `CONFIG_GPIO=y` - Enable GPIO driver

Device tree overlay is configured in `boards/nrf54l15dk_nrf54l15_cpuapp.overlay`

## 🎼 How It Works

1. **Initialization**: 
   - PWM device is initialized for buzzer control
   - GPIO pin 2.00 is configured as output for LED

2. **Melody Playback**:
   - Each note in the melody array is played sequentially
   - LED turns ON when a note plays
   - LED turns OFF during pauses and between notes

3. **Note Generation**:
   - Each note frequency is converted to PWM period
   - 90% duty cycle provides maximum volume
   - Note duration controls timing

4. **Continuous Loop**:
   - Plays Jingle Bells melody
   - Waits 2 seconds
   - Plays a simple beep
   - Waits 5 seconds before repeating

## 📁 Project Structure

```
buzzer_nrf54/
├── src/
│   └── main.c              # Main application code
├── boards/
│   └── nrf54l15dk_nrf54l15_cpuapp.overlay  # Device tree overlay
├── CMakeLists.txt          # Build configuration
├── prj.conf                # Zephyr configuration
└── README.md              # This file
```

## 🎯 Key Functions

- `play_melody()`: Plays the complete Jingle Bells melody
- `play_note()`: Plays a single note with specified frequency and duration
- `simple_beep()`: Plays a simple beep sound
- `led_on()` / `led_off()`: Control LED state

## 📝 Code Highlights

- **Musical Notes**: Comprehensive note frequency definitions (B0-C8)
- **Note Durations**: Standard musical timing (whole, half, quarter, eighth, sixteenth notes)
- **PWM Control**: Precise frequency generation using PWM period calculation
- **LED Sync**: Real-time LED control synchronized with music playback

## 🎬 Demo

The application continuously plays:
1. Jingle Bells melody with LED blinking
2. Brief pause
3. Simple beep with LED flash
4. Longer pause before repeating

### Video Demonstration

Watch the buzzer in action with synchronized LED blinking:

<video src="jingle_bells.mp4" controls width="640"></video>

The video shows the buzzer playing the Jingle Bells melody with the LED at pin 2.00 blinking in perfect synchronization with the music.

## 🔧 Troubleshooting

- **No sound**: Check buzzer connection to P1.08
- **LED not blinking**: Verify LED connection to P2.00
- **Build errors**: Ensure Zephyr SDK and toolchain are properly installed

## 📚 Resources

- [Zephyr RTOS Documentation](https://docs.zephyrproject.org/)
- [nRF54 Series Documentation](https://infocenter.nordicsemi.com/)
- [PWM Driver API](https://docs.zephyrproject.org/latest/hardware/peripherals/pwm.html)
- [GPIO Driver API](https://docs.zephyrproject.org/latest/hardware/peripherals/gpio.html)

---

**Enjoy your musical buzzer with synchronized LED! 🎄🔔✨**

