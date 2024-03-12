#include "mvg.h"
#include "httpRequest.h"

namespace mvg {
String station_id_garching = "";
String station_id_oly = "";


String getTrainStationId(String station) {
  String url = String("https://www.mvg.de/api/fib/v2/location?query=") + station;
  String station_data = httpRequest::httpRequest(url.c_str());
  // Serial.println(station_data);

  //Less memory consumption than complete JSON deserialization
  int index = station_data.indexOf("globalId");
  if (index >= 0) {
    return station_data.substring(index + 11, index + 23);
  }
  return "Invalid";
}

String getTrainDeprature(String from, String to) {
  String url = String("https://www.mvg.de/api/fib/v2/connection?originStationGlobalId=")
               + from
               + String("&destinationStationGlobalId=")
               + to
               + String("&routingDateTimeIsArrival=false&transportTypes=SCHIFF,RUFTAXI,BAHN,UBAHN,TRAM,SBAHN,BUS,REGIONAL_BUS");
  Serial.println(url);
  String departure_data = httpRequest::httpRequest(url.c_str());

  //Less memory consumption than complete JSON deserialization
  int index = departure_data.indexOf("plannedDeparture");
  if (index >= 0) {
    return departure_data.substring(index + 29, index + 44);
  }
  return "Invalid";
}

void showMVGDepartureInfo(String from, String to) {
  if (station_id_garching.length() == 0) {
    Serial.println("Loading Station Names");
    station_id_garching = getTrainStationId(from);
    station_id_oly = getTrainStationId(to);
  }
  Serial.println(getTrainDeprature(station_id_garching, station_id_oly));
}

}