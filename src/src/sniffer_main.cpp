#include <Arduino.h>
#include "targets.h"

// Define TARGET_SNIFFER if not already defined
#ifndef TARGET_SNIFFER
#define TARGET_SNIFFER 1
#endif

#include "common.h"
#include "logging.h"
#include "helpers.h"
#include "OTA.h"

#if defined(RADIO_SX128X)
#include "SX1280Driver.h"
SX1280Driver Radio;
#elif defined(RADIO_SX127X)
#include "SX127xDriver.h"  
SX127xDriver Radio;
#else
#error "No radio driver defined"
#endif

// TFT Display
#ifdef TFT_CS
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
#define HAS_DISPLAY
#endif

// Packet statistics
struct PacketStats {
    uint32_t totalPackets = 0;
    uint32_t syncPackets = 0;
    uint32_t dataPackets = 0;
    uint32_t tlmPackets = 0;
    uint32_t mspPackets = 0;
    uint32_t unknownPackets = 0;
    int8_t lastRSSI = -127;
    int8_t lastSNR = -128;
    uint32_t lastFreq = 0;
    uint8_t lastUID[6] = {0};
    bool hasUID = false;
} stats;

// Timing
uint32_t lastDisplayUpdate = 0;
uint32_t lastPacketTime = 0;
const uint32_t DISPLAY_UPDATE_INTERVAL = 100; // 100ms

// FHSS
uint32_t currentFreq = 0;
uint8_t currentFHSSIndex = 0;
bool isListening = false;

// Buffer for received packets
uint8_t packetBuffer[256];
uint8_t packetLength = 0;

void initDisplay() {
#ifdef HAS_DISPLAY
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(0, 0);
    tft.println("ELRS Packet Sniffer");
    tft.println("LilyGO T3-S3");
    tft.println("Initializing...");
#endif
}

void updateDisplay() {
#ifdef HAS_DISPLAY
    static char buffer[128];
    
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0);
    tft.setTextColor(TFT_CYAN);
    tft.println("ELRS Packet Sniffer");
    
    tft.setTextColor(TFT_WHITE);
    sprintf(buffer, "Freq: %lu Hz", stats.lastFreq);
    tft.println(buffer);
    
    sprintf(buffer, "Total: %lu", stats.totalPackets);
    tft.println(buffer);
    
    sprintf(buffer, "SYNC: %lu", stats.syncPackets);
    tft.println(buffer);
    
    sprintf(buffer, "DATA: %lu", stats.dataPackets);
    tft.println(buffer);
    
    sprintf(buffer, "TLM:  %lu", stats.tlmPackets);
    tft.println(buffer);
    
    sprintf(buffer, "MSP:  %lu", stats.mspPackets);
    tft.println(buffer);
    
    sprintf(buffer, "RSSI: %d dBm", stats.lastRSSI);
    tft.println(buffer);
    
    sprintf(buffer, "SNR:  %d dB", stats.lastSNR);
    tft.println(buffer);
    
    if (stats.hasUID) {
        sprintf(buffer, "UID: %02X%02X%02X%02X%02X%02X", 
                stats.lastUID[0], stats.lastUID[1], stats.lastUID[2],
                stats.lastUID[3], stats.lastUID[4], stats.lastUID[5]);
        tft.println(buffer);
    }
    
    // Status
    tft.setTextColor(isListening ? TFT_GREEN : TFT_RED);
    tft.println(isListening ? "LISTENING" : "IDLE");
#endif
}

void processPacket() {
    OTA_Packet_s *packet = (OTA_Packet_s*)packetBuffer;
    
    stats.totalPackets++;
    stats.lastRSSI = Radio.GetLastPacketRSSI();
    stats.lastSNR = Radio.GetLastPacketSNRRaw();
    stats.lastFreq = currentFreq;
    lastPacketTime = millis();
    
    // Determine packet type
    uint8_t packetType = packet->std.type;
    
    switch (packetType) {
        case PACKET_TYPE_SYNC:
            stats.syncPackets++;
            // Extract UID from SYNC packet
            if (packetLength >= sizeof(OTA_Sync_s)) {
                stats.lastUID[4] = packet->std.sync.UID4;
                stats.lastUID[5] = packet->std.sync.UID5;
                stats.hasUID = true;
            }
            break;
        case PACKET_TYPE_RCDATA:
        case PACKET_TYPE_DATA:
            stats.dataPackets++;
            break;
        case PACKET_TYPE_LINKSTATS:
            stats.tlmPackets++;
            break;
        default:
            if (packetType == 0x3) { // MSP packet type might be different
                stats.mspPackets++;
            } else {
                stats.unknownPackets++;
            }
            break;
    }
    
    // Log packet info
    DBGLN("PKT: Type=%d, RSSI=%d, SNR=%d, Len=%d", 
          packetType, stats.lastRSSI, stats.lastSNR, packetLength);
}

void scanFrequencies() {
    static uint32_t lastScan = 0;
    static uint8_t scanIndex = 0;
    
    if (millis() - lastScan > 20) { // Change frequency every 20ms for better scanning
        // Use ELRS 2.4GHz frequency table (simplified)
        const uint32_t frequencies[] = {
            2400400000UL, 2401400000UL, 2402400000UL, 2403400000UL, 2404400000UL,
            2405400000UL, 2406400000UL, 2407400000UL, 2408400000UL, 2409400000UL,
            2410400000UL, 2411400000UL, 2412400000UL, 2413400000UL, 2414400000UL,
            2415400000UL, 2416400000UL, 2417400000UL, 2418400000UL, 2419400000UL,
            2420400000UL, 2421400000UL, 2422400000UL, 2423400000UL, 2424400000UL,
            2425400000UL, 2426400000UL, 2427400000UL, 2428400000UL, 2429400000UL,
            2430400000UL, 2431400000UL, 2432400000UL, 2433400000UL, 2434400000UL,
            2435400000UL, 2436400000UL, 2437400000UL, 2438400000UL, 2439400000UL,
            2440400000UL, 2441400000UL, 2442400000UL, 2443400000UL, 2444400000UL,
            2445400000UL, 2446400000UL, 2447400000UL, 2448400000UL, 2449400000UL,
            2450400000UL, 2451400000UL, 2452400000UL, 2453400000UL, 2454400000UL,
            2455400000UL, 2456400000UL, 2457400000UL, 2458400000UL, 2459400000UL,
            2460400000UL, 2461400000UL, 2462400000UL, 2463400000UL, 2464400000UL,
            2465400000UL, 2466400000UL, 2467400000UL, 2468400000UL, 2469400000UL,
            2470400000UL, 2471400000UL, 2472400000UL, 2473400000UL, 2474400000UL,
            2475400000UL, 2476400000UL, 2477400000UL, 2478400000UL, 2479400000UL
        };
        
        const uint8_t numFreqs = sizeof(frequencies) / sizeof(frequencies[0]);
        currentFreq = frequencies[scanIndex % numFreqs];
        
        Radio.SetFrequencyHz(currentFreq);
        Radio.StartReceiving();
        
        scanIndex++;
        lastScan = millis();
    }
}

void setup() {
    Serial.begin(420000);
    
    // Initialize GPIO pins
    pinMode(GPIO_PIN_BUTTON, INPUT_PULLUP);
    pinMode(GPIO_PIN_LED, OUTPUT);
    digitalWrite(GPIO_PIN_LED, LOW);
    
    // Initialize logging
    #ifdef PLATFORM_ESP32
    logging_init();
    #endif
    DBGLN("ELRS Packet Sniffer Starting...");
    
    // Initialize display
    initDisplay();
    
    // Initialize radio
    if (!Radio.Begin()) {
        DBGLN("Radio initialization failed!");
        return;
    }
    
    // Configure for 2.4GHz ELRS
    RadioHAL.IsrCallback = []() { Radio.IsrCallback(); };
    
    // Set basic configuration - we'll scan frequencies manually
    Radio.SetFrequencyHz(2400400000);
    Radio.SetOutputPower(0); // Minimum power for sniffer
    
    isListening = true;
    
    DBGLN("Sniffer initialized");
    
#ifdef HAS_DISPLAY
    tft.fillScreen(TFT_BLACK);
    updateDisplay();
#endif
}

void loop() {
    // Handle radio interrupt and packet reception
    Radio.HandleTXLQIandRSSI();
    
    // Check for received packet
    if (Radio.GetIrqFlags()) {
        uint8_t irqStatus = Radio.GetIrqFlags();
        Radio.ClearIrqFlags();
        
        if (irqStatus & 0x02) { // RX done
            packetLength = Radio.GetLastPacketLen();
            if (packetLength > 0 && packetLength < 256) {
                Radio.GetLastPacketStats();
                processPacket();
            }
        }
        
        // Restart receiving
        Radio.StartReceiving();
    }
    
    // Scan frequencies
    scanFrequencies();
    
    // Update display
    if (millis() - lastDisplayUpdate > DISPLAY_UPDATE_INTERVAL) {
        updateDisplay();
        lastDisplayUpdate = millis();
    }
    
    // Handle button press (reset stats)
    if (digitalRead(GPIO_PIN_BUTTON) == LOW) {
        delay(50); // Debounce
        if (digitalRead(GPIO_PIN_BUTTON) == LOW) {
            memset(&stats, 0, sizeof(stats));
            DBGLN("Stats reset");
            while (digitalRead(GPIO_PIN_BUTTON) == LOW) {
                delay(10);
            }
        }
    }
    
    // Blink LED when packets received
    if (millis() - lastPacketTime < 100) {
        digitalWrite(GPIO_PIN_LED, HIGH);
    } else {
        digitalWrite(GPIO_PIN_LED, LOW);
    }
    
    delay(1);
}