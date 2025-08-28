// TFT_eSPI configuration for LilyGO T3-S3

#define ST7789_DRIVER

#define TFT_WIDTH  135
#define TFT_HEIGHT 240

// Pin definitions for LilyGO T3-S3 
#define TFT_MOSI 15
#define TFT_SCLK 14
#define TFT_CS   9
#define TFT_DC   11
#define TFT_RST  12

// Fonts
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

#define SMOOTH_FONT

// Color depth
#define TFT_RGB_ORDER TFT_BGR

// SPI
#define SPI_FREQUENCY  40000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000