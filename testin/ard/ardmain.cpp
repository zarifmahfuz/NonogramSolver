#include <Arduino.h>
#include "serialcom.h"
#include <MCUFRIEND_kbv.h>
#include <SD.h>
#define SD_CS 10

// Physical dimensions of the tft display (# of pixels)
#define DISPLAY_WIDTH  480
#define DISPLAY_HEIGHT 320

// Touch screen pins, obtained from the documentation
#define YP A3  // Must be an analog pin, use "An" notation!
#define XM A2  // Must be an analog pin, use "An" notation!
#define YM 9  // Can be a digital pin
#define XP 8  // Can be a digital pin

// Dimensions of the part allocated to the map display
#define MAP_DISP_WIDTH (DISPLAY_WIDTH - 60)
#define MAP_DISP_HEIGHT DISPLAY_HEIGHT

#define REST_START_BLOCK 4000000
#define NUM_RESTAURANTS 1066

// Calibration data for the touch screen, obtained from documentation
// the minimum/maximum possible readings from the touch point
#define TS_MINX 100
#define TS_MINY 120
#define TS_MAXX 940
#define TS_MAXY 920

// Thresholds to determine if there was a touch
#define MINPRESSURE   10
#define MAXPRESSURE 1000

Sd2Card card;

MCUFRIEND_kbv tft;

void setup() {
    init();
    Serial.begin(9600);
    Serial.flush();


    // tft display initialization
    uint16_t ID = tft.readID();
    tft.begin(ID);

    tft.fillScreen(TFT_BLACK);
    tft.setRotation(1);

    // SD card initialization for raw reads
    Serial.print("Initializing SPI communication for raw reads...");
    if (!card.init(SPI_HALF_SPEED, SD_CS)) {
        Serial.println("failed! Is the card inserted properly?");
        while (true) {}
    } else {
        Serial.println("OK!");
    }
}

int main(){
    setup();

    // Initialize dimensions array as -1 to hold information from communication
    // dimensions[0] is row size
    // dimensions[1] is column size
    // dimensions[2] is runtime
    // dimensions[3] is marker for the while loop
    int dimensions[4];
    dimensions[1] = -1;
    dimensions[0] = -1;
    dimensions[2] = -1;
    dimensions[3] = -1;

    // Initialize nonogram matrix
    int** nonogram;

    // Communication until it is successful (when dimensions[3] is no longer -1)
    while (dimensions[3] == -1){
    nonogram = solvedNono(dimensions);
    }

    // Bunch of debug statements to see what was going on. We can remove this
    tft.setCursor(0,0);
    tft.setTextSize(2);

    tft.print(dimensions[0]);
    tft.setCursor(0,20);
    tft.print(dimensions[1]);

    tft.setCursor(0, 50);

    for (int i = 0; i < dimensions[0]; i++) {
        tft.print(nonogram[i][0]);
        tft.print("  ");
        tft.print(nonogram[i][1]);
        tft.print(nonogram[i][2]);
        tft.setCursor(0, 70+i*20);
    }

    tft.setCursor(100, 0);
    tft.print(dimensions[2]);

    Serial.flush();  // Don't forget to flush at the end
    return 0;
}