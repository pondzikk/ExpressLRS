#pragma once

// Hardware definitions for LilyGO T3-S3 ELRS Packet Sniffer
#define TARGET_LILYGO_T3S3_SNIFFER

// Serial
#define GPIO_PIN_RCSIGNAL_RX 44
#define GPIO_PIN_RCSIGNAL_TX 43

// Radio - SX1280 2.4GHz
#define GPIO_PIN_BUSY 34
#define GPIO_PIN_DIO0 33
#define GPIO_PIN_DIO1 33
#define GPIO_PIN_MISO 3
#define GPIO_PIN_MOSI 6
#define GPIO_PIN_NSS 7
#define GPIO_PIN_RST 8
#define GPIO_PIN_SCK 5

// Radio power control
#define GPIO_PIN_PA_ENABLE 35
#define GPIO_PIN_RX_ENABLE 21
#define GPIO_PIN_TX_ENABLE 10

// Display - TFT
#define TFT_CS 9
#define TFT_DC 11
#define TFT_MOSI 15
#define TFT_RST 12
#define TFT_SCLK 14

// UI
#define GPIO_PIN_BUTTON 0
#define GPIO_PIN_LED 37
#define GPIO_PIN_BUZZER 13

// Platform specific
#define PLATFORM_ESP32_S3
#define TARGET_SNIFFER 1
#define RADIO_SX128X 1

// Power settings for sniffer mode (lower power)
#define MinPower POWERMGNT::PWR_10mW
#define MaxPower POWERMGNT::PWR_25mW
#define DefaultPower POWERMGNT::PWR_10mW

// Use hardware DCDC if available
#define OPT_USE_HARDWARE_DCDC true