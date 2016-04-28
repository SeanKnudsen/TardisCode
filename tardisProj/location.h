#ifndef _LOCATION_H
#define _LOCATION_H
//#define SERIAL_DEBUG 1

#include <Adafruit_GPS.h>

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO  false

#define RADIUS_KM 6371
#define RADIUS_MI 3959
#define TO_RAD (3.1415926536 / 180)

#define TARGET_LAT 37.80956388888889
#define TARGET_LON 122.18388055555556
#define FOUND_DISTANCE 35 // if you're within 35 feet, you've got it!

/* Example coordinates */
// Sean's house:         37.80956388888889, 122.18388055555556
// Jamieson Rach Winery: 38.22041388888889, 122.22920833333333

class Location {
  public:
    Location();
    void setup();
    
    void collectGPS();
    bool hasFix();
    void setTarget(float lat, float lon);

    void useInterrupt(boolean v);
    float Latitude;
    int LatDeg;
    float LatMin;
    
    float Longitude;
    
    int LonDeg;
    float LonMin;
    
    float Altitude;
    float Distance;

    uint8_t Hour;
    uint8_t Minute; 
    uint8_t Year;
    uint8_t Month;
    uint8_t Day;

// set target
// atTarget


    Adafruit_GPS gps;
   
  private:
    float targetLat, targetLon;
    boolean usingInterrupt;

    double convertDegMinToDecDeg(float degMin);
    double dist(double lat2, double lon2);
    int getDegrees(float degMin);
    float getMinutes(float degMin);
};
#endif
