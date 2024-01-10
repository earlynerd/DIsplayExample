#include <Arduino.h>
#include "Adafruit_TinyUSB.h"
#include <SSD1322_for_Adafruit_GFX.h>
#include <stdio.h>

#define OLED_CLK 18
#define OLED_MOSI 19
#define OLED_CS 17
#define OLED_DC 20

Adafruit_SSD1322 display(256, 64, &SPI, OLED_DC, 22, OLED_CS, 20000000);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH 16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
    {0b00000000, 0b11000000,
     0b00000001, 0b11000000,
     0b00000001, 0b11000000,
     0b00000011, 0b11100000,
     0b11110011, 0b11100000,
     0b11111110, 0b11111000,
     0b01111110, 0b11111111,
     0b00110011, 0b10011111,
     0b00011111, 0b11111100,
     0b00001101, 0b01110000,
     0b00011011, 0b10100000,
     0b00111111, 0b11100000,
     0b00111111, 0b11110000,
     0b01111100, 0b11110000,
     0b01110000, 0b01110000,
     0b00000000, 0b00110000};

void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h);
void testdrawchar(void);
void testdrawcircle(void);
void testfillrect(void);
void testdrawtriangle(void);
void testfilltriangle(void);
void testdrawroundrect(void);
void testfillroundrect(void);
void testdrawrect(void);
void testdrawline();


void setup()
{
    Serial.begin(115200);
    pinMode(OLED_CS, OUTPUT);
    digitalWrite(OLED_CS, HIGH);
    pinMode(OLED_DC, OUTPUT);
    digitalWrite(OLED_DC, HIGH);

    SPI.setTX(OLED_MOSI);
    SPI.setRX(16);
    SPI.setSCK(OLED_CLK);
    if (!display.begin(0, false))
    {
        Serial.println("Unable to initialize OLED");
        while (1)
            delay(1);
    }
    else
        Serial.println("Display init!");

    display.setRotation(0);
    display.setContrast(255);
    // display.setMasterCurrent(0xff);
    // display.setPreChargeVoltage(0x1f);
    display.clearDisplay();
    display.display();

    display.setCursor(10, 10);
    display.setTextSize(1);
    display.print("It Work");
    display.display();
    display.clearDisplay();
    display.display();

    // draw many lines
    testdrawline();
    display.display();
    delay(1000);
    display.clearDisplay();

    // draw rectangles
    testdrawrect();
    display.display();
    delay(1000);
    display.clearDisplay();

    // draw multiple rectangles
    testfillrect();
    display.display();
    delay(1000);
    display.clearDisplay();

    // draw mulitple circles
    testdrawcircle();
    display.display();
    delay(1000);
    display.clearDisplay();

    // draw a SSD1322_WHITE circle, 10 pixel radius
    display.fillCircle(display.width() / 2, display.height() / 2, 10, SSD1322_WHITE);
    display.display();
    delay(1000);
    display.clearDisplay();

    testdrawroundrect();
    delay(1000);
    display.clearDisplay();

    testfillroundrect();
    delay(1000);
    display.clearDisplay();

    testdrawtriangle();
    delay(1000);
    display.clearDisplay();

    testfilltriangle();
    delay(1000);
    display.clearDisplay();

    // draw the first ~12 characters in the font
    testdrawchar();
    display.display();
    delay(1000);
    display.clearDisplay();

    for (uint8_t rot = 0; rot < 4; rot++)
    {
        display.setRotation(rot);
        display.clearDisplay();
        // text display tests
        display.setTextSize(1);
        display.setTextColor(SSD1322_WHITE);
        display.setCursor(0, 0);
        display.println("Hello, world!");
        display.setTextColor(SSD1322_BLACK, SSD1322_WHITE); // 'inverted' text
        display.println(3.141592);
        display.setTextSize(2);
        display.setTextColor(SSD1322_WHITE);
        display.print("0x");
        display.println(0xDEADBEEF, HEX);
        display.display();
        delay(1000);
    }

    display.setRotation(0);

    // miniature bitmap display
    display.clearDisplay();
    display.drawBitmap(30, 16, logo16_glcd_bmp, 16, 16, 1);
    display.display();

    // invert the display
    display.invertDisplay(true);
    delay(1000);
    display.invertDisplay(false);
    delay(1000);

    // draw a bitmap icon and 'animate' movement
    testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
}

void loop()
{
    yield();
}

void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h)
{
    uint8_t icons[NUMFLAKES][3];
    randomSeed(666); // whatever seed

    // initialize
    for (uint8_t f = 0; f < NUMFLAKES; f++)
    {
        icons[f][XPOS] = random(display.width());
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = random(5) + 1;

        Serial.print("x: ");
        Serial.print(icons[f][XPOS], DEC);
        Serial.print(" y: ");
        Serial.print(icons[f][YPOS], DEC);
        Serial.print(" dy: ");
        Serial.println(icons[f][DELTAY], DEC);
    }

    while (1)
    {
        // draw each icon
        for (uint8_t f = 0; f < NUMFLAKES; f++)
        {
            display.drawBitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, f + 1);
        }
        display.display();
        delay(10);

        // then erase it + move it
        for (uint8_t f = 0; f < NUMFLAKES; f++)
        {
            display.drawBitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, SSD1322_BLACK);
            // move it
            icons[f][YPOS] += icons[f][DELTAY];
            // if its gone, reinit
            if (icons[f][YPOS] > display.height())
            {
                icons[f][XPOS] = random(display.width());
                icons[f][YPOS] = 0;
                icons[f][DELTAY] = random(5) + 1;
            }
        }
    }
}

void testdrawchar(void)
{
    display.setTextSize(1);
    display.setTextWrap(false);
    display.setTextColor(SSD1322_WHITE);
    display.setCursor(0, 0);

    for (uint8_t i = 0; i < 168; i++)
    {
        if (i == '\n')
            continue;
        display.write(i);
        if ((i > 0) && (i % 21 == 0))
            display.println();
    }
    display.display();
}

void testdrawcircle(void)
{
    for (uint8_t i = 0; i < display.height(); i += 2)
    {
        display.drawCircle(display.width() / 2, display.height() / 2, i, i % 15 + 1);
        display.display();
    }
}

void testfillrect(void)
{
    uint8_t color = 1;
    for (uint8_t i = 0; i < display.height() / 2; i += 3)
    {
        // alternate colors
        display.fillRect(i, i, display.width() - i * 2, display.height() - i * 2, i % 15 + 1);
        display.display();
        color++;
    }
}

void testdrawtriangle(void)
{
    for (uint16_t i = 0; i < min(display.width(), display.height()) / 2; i += 5)
    {
        display.drawTriangle(display.width() / 2, display.height() / 2 - i,
                             display.width() / 2 - i, display.height() / 2 + i,
                             display.width() / 2 + i, display.height() / 2 + i, i % 15 + 1);
        display.display();
    }
}

void testfilltriangle(void)
{
    uint8_t color = SSD1322_WHITE;
    for (int16_t i = min(display.width(), display.height()) / 2; i > 0; i -= 5)
    {
        display.fillTriangle(display.width() / 2, display.height() / 2 - i,
                             display.width() / 2 - i, display.height() / 2 + i,
                             display.width() / 2 + i, display.height() / 2 + i, i % 15 + 1);
        display.display();
    }
}

void testdrawroundrect(void)
{
    for (int i = 0; i < display.height() / 3 - 2; i += 2)
    {
        display.drawRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, i % 15 + 1);
        display.display();
    }
}

void testfillroundrect(void)
{
    uint8_t color = SSD1322_WHITE;
    for (uint8_t i = 0; i < display.height() / 3 - 2; i += 2)
    {
        display.fillRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, i % 15 + 1);
        display.display();
    }
}

void testdrawrect(void)
{
    for (uint8_t i = 0; i < display.height() / 2; i += 2)
    {
        display.drawRect(i, i, display.width() - 2 * i, display.height() - 2 * i, i % 15 + 1);
        display.display();
    }
}

void testdrawline()
{
    for (int i = 0; i < display.width(); i += 4)
    {
        display.drawLine(0, 0, i, display.height() - 1, SSD1322_WHITE);
        display.display();
    }
    for (int i = 0; i < display.height(); i += 4)
    {
        display.drawLine(0, 0, display.width() - 1, i, SSD1322_WHITE);
        display.display();
    }
    delay(250);

    display.clearDisplay();
    for (int i = 0; i < display.width(); i += 4)
    {
        display.drawLine(0, display.height() - 1, i, 0, SSD1322_WHITE);
        display.display();
    }
    for (int i = display.height() - 1; i >= 0; i -= 4)
    {
        display.drawLine(0, display.height() - 1, display.width() - 1, i, SSD1322_WHITE);
        display.display();
    }
    delay(250);

    display.clearDisplay();
    for (int i = display.width() - 1; i >= 0; i -= 4)
    {
        display.drawLine(display.width() - 1, display.height() - 1, i, 0, SSD1322_WHITE);
        display.display();
    }
    for (int i = display.height() - 1; i >= 0; i -= 4)
    {
        display.drawLine(display.width() - 1, display.height() - 1, 0, i, SSD1322_WHITE);
        display.display();
    }
    delay(250);

    display.clearDisplay();
    for (int i = 0; i < display.height(); i += 4)
    {
        display.drawLine(display.width() - 1, 0, 0, i, SSD1322_WHITE);
        display.display();
    }
    for (int i = 0; i < display.width(); i += 4)
    {
        display.drawLine(display.width() - 1, 0, i, display.height() - 1, SSD1322_WHITE);
        display.display();
    }
    delay(250);
}
