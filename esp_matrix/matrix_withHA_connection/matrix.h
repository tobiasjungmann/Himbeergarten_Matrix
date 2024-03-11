#ifndef MATRIX_H
#define MATRIX_H
namespace matrix{
void setup();
void displayScrollText(char* message);
void updateBrightness(int analogValue);

void displayStaticText(const char* text);

void matrixActivateSleep();
}

#endif  // MATRIX
