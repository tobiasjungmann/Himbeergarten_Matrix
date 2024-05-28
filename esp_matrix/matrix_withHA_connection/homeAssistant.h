#ifndef HOME_ASSISTANT_H
#define HOME_ASSISTANT_H

namespace homeAssistant {

enum MatrixState {
  SPOTIFY,
  TRAIN,
  TEMP_INSIDE,
  TEMP_OUTSIDE,
  TIME,
  HUMIDITY
};

//void getStatesToShow();
void showTemperature();
void showHumidityInPercent();

/**
* Adds the necessary sensors to the Home assistant instance which are available for this matrix esp
*/
void setup(IPAddress forwarderAddress);

/**
* Queries the state of the sensors which represent the different modes of the matrix
*/
void updateSelectedStates();
std::vector<MatrixState> getStatesToShow();
}

#endif  //HOME_ASSISTANT_H