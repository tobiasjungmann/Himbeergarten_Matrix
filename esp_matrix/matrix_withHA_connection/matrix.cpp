#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>

#include "matrix.h"
#include "gpio_config.h"

namespace matrix {

// Matrix
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CHAR_SPACING 4  // pixels between characters
//MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_Parola mx = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// states whether something is currently displayed in the matrix
boolean matrix_cleared = true;


void setup(){
mx.begin();
}

void displayStaticText(const char* text){
  mx.displayText(text, PA_CENTER, 25, 500, PA_PRINT, PA_PRINT);
  mx.displayAnimate();
  matrix_cleared = false;
  }

void displayScrollText(char* message) {
  mx.displayClear();
  mx.displayScroll(message, PA_RIGHT, PA_SCROLL_LEFT, 48);
  while (!mx.displayAnimate()) { ; }
  matrix_cleared = false;
}

void updateBrightness(int analogValue) {
  float brightness = analogValue / 256;
  mx.setIntensity(brightness);
}

void matrixActivateSleep(){
   if (!matrix_cleared) {
      matrix_cleared = true;
      mx.displayClear();
      // todo  P.displayShutdown(true); if it is switchd off?
    }
}
}

