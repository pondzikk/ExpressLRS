# LilyGO T3-S3 ExpressLRS Packet Sniffer

## Overview

This project adds support for using the LilyGO T3-S3 board as an ExpressLRS packet sniffer. The sniffer can monitor and analyze 2.4GHz ExpressLRS traffic in real-time, displaying statistics and packet information on the built-in TFT display.

## Features

- **Real-time packet monitoring**: Sniffs ExpressLRS 2.4GHz packets across frequency bands
- **TFT Display**: Shows live statistics including:
  - Current frequency
  - Total packets received
  - Packet type breakdown (SYNC, DATA, TLM, MSP)
  - RSSI and SNR values
  - UID information when available
- **Frequency scanning**: Automatically scans through ELRS 2.4GHz frequency table
- **Interactive controls**: Button to reset statistics
- **LED indication**: Visual feedback when packets are received
- **Serial debugging**: Detailed packet information via UART

## Hardware Requirements

- **LilyGO T3-S3 Board** with:
  - ESP32-S3 microcontroller
  - SX1280 2.4GHz LoRa radio
  - 135x240 TFT display
  - Built-in antenna

## Pin Configuration

The sniffer uses the following pin configuration for the LilyGO T3-S3:

### Radio (SX1280)
- SCK: GPIO 5
- MISO: GPIO 3  
- MOSI: GPIO 6
- NSS: GPIO 7
- RST: GPIO 8
- BUSY: GPIO 34
- DIO1: GPIO 33

### Display (TFT)
- SCK: GPIO 14
- MOSI: GPIO 15
- CS: GPIO 9
- DC: GPIO 11
- RST: GPIO 12

### Controls
- Button: GPIO 0 (Boot button)
- LED: GPIO 37

### Power Control
- PA Enable: GPIO 35
- RX Enable: GPIO 21
- TX Enable: GPIO 10

## Building and Flashing

### Prerequisites

1. Install PlatformIO Core or PlatformIO IDE
2. Clone the ExpressLRS repository
3. Navigate to the `src` directory

### Build Commands

```bash
# Build the firmware
pio run -e LilyGO_T3S3_2400_Sniffer_via_UART

# Build and upload via USB
pio run -e LilyGO_T3S3_2400_Sniffer_via_UART --target upload

# Build and upload via WiFi (if supported)
pio run -e LilyGO_T3S3_2400_Sniffer_via_WIFI --target upload

# Monitor serial output
pio device monitor -b 420000
```

## Usage

1. **Power on**: Connect the T3-S3 via USB or battery
2. **Monitor display**: The TFT will show real-time packet statistics
3. **Reset stats**: Press the boot button (GPIO 0) to reset counters
4. **View details**: Connect to serial monitor for detailed packet logs

### Display Information

The TFT display shows:

```
ELRS Packet Sniffer
Freq: 2400400000 Hz
Total: 1234
SYNC: 567
DATA: 432
TLM: 123
MSP: 12
RSSI: -65 dBm
SNR: 8 dB
UID: 1234567890AB
LISTENING
```

### Serial Output

Detailed packet information is available via serial console:
```
ELRS Packet Sniffer Starting...
PKT: Type=2, RSSI=-67, SNR=9, Len=8
PKT: Type=1, RSSI=-65, SNR=10, Len=13
```

## Packet Types

The sniffer classifies packets into the following types:

- **SYNC (Type 2)**: Synchronization packets containing timing and frequency information
- **DATA (Type 1)**: RC control data and channel information  
- **TLM (Type 0)**: Telemetry data from receiver to transmitter
- **MSP (Type 3)**: MSP (MultiWii Serial Protocol) packets
- **Unknown**: Unrecognized packet types

## Frequency Scanning

The sniffer automatically cycles through the ExpressLRS 2.4GHz frequency table:
- Range: 2400.4 MHz to 2479.4 MHz
- Step: 1 MHz
- 80 total frequencies
- Scan rate: 10ms per frequency

## Technical Details

### Software Architecture

The sniffer implementation consists of:

1. **Main Application** (`sniffer_main.cpp`): Core packet processing and display
2. **Hardware Definition** (`LilyGO_T3S3_Sniffer.h`): Pin mappings and configuration
3. **Display Config** (`User_Setup.h`): TFT_eSPI configuration for the display
4. **Build Config** (`unified.ini`): PlatformIO build environment

### Packet Processing

- Uses existing ExpressLRS radio drivers (SX1280Driver)
- Leverages OTA packet structure definitions
- Implements frequency hopping sequence scanning
- Provides real-time RSSI/SNR measurements

### Performance

- Packet processing: Real-time with minimal latency
- Display updates: 100ms intervals (10 FPS)
- Frequency scanning: 10ms per frequency
- Memory usage: Optimized for ESP32-S3

## Troubleshooting

### Build Issues

1. **Platform not found**: Run `pio platform install espressif32@6.4.0`
2. **Missing libraries**: Libraries will be auto-downloaded during build
3. **TFT not working**: Check User_Setup.h pin configuration

### Runtime Issues

1. **No packets detected**: 
   - Verify antenna connection
   - Check frequency range
   - Ensure ELRS transmitter is active in area

2. **Display not working**:
   - Check power connections
   - Verify TFT pin configuration
   - Try different display brightness

3. **Poor reception**:
   - Improve antenna positioning
   - Move closer to ELRS equipment
   - Check for interference sources

## Customization

### Modifying Frequencies

Edit the frequency table in `sniffer_main.cpp`:

```cpp
const uint32_t frequencies[] = {
    2400400000, 2401400000, // Add/modify frequencies
    // ... more frequencies
};
```

### Display Layout

Modify the `updateDisplay()` function to change the information shown on screen.

### Packet Filtering

Add filters in `processPacket()` to focus on specific packet types or sources.

## Contributing

This sniffer implementation follows ExpressLRS coding standards:

1. Minimal changes to core codebase
2. Reuse existing radio drivers and packet structures  
3. Platform-specific configuration in target headers
4. Modular design for easy extension

## License

This code is released under the same license as ExpressLRS (GPL-3.0).

## Support

For issues specific to the LilyGO T3-S3 sniffer:
1. Check this README for troubleshooting
2. Review serial console output for debug information
3. Submit issues with detailed logs and hardware information

For general ExpressLRS questions, please use the main ExpressLRS support channels.