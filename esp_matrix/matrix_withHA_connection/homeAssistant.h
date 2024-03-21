#ifndef HOME_ASSISTANT_H
#define HOME_ASSISTANT_H

namespace homeAssistant {

enum MatrixState {
  SPOTIFY,
  TRAIN,
  TEMP_INSIDE,
  TEMP_OUTSIDE,
  TIME
};

void showTemperature();

/**
* Adds the necessary sensors to the Home assistant instance which are available for this matrix esp
*/
void setup();

/**
* Queries the state of the sensors which represent the different modes of the matrix
*/
void updateSelectedStates();
bool isStateVisible(MatrixState state);
}

#endif  //HOME_ASSISTANT_H