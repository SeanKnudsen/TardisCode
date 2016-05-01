#include "location.h"



//  Constructor
Location::Location() :
  gps(Adafruit_GPS(&Serial1))
  {
  }



// Call this from setup()
void Location::setup()
{
  usingInterrupt = true;
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  Serial.begin(115200);
  //#if SERIAL_DEBUG
  Serial.println("Entered -Location-Setup");
  //#endif
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  gps.begin(9600);

  gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  
  // Set the update rate
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate

  useInterrupt(true);
}


void Location::useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

void Location::collectGPS()
{
   char c = gps.read();
  // if you want to debug, this is a good time to do it!
  if ((c) && (GPSECHO)) {}

  //Serial.print(c);
  //Serial.println("collectGPS()");
  // if a sentence is received, we can check the checksum, parse it...
  if (gps.newNMEAreceived()) 
  {
    if (gps.parse(gps.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
    {      
        //Serial.println("parsed OK");
      if(hasFix()){
        Latitude = convertDegMinToDecDeg(gps.latitude);
        LatDeg = getDegrees(gps.latitude);
        LatMin = getMinutes(gps.latitude);

        Longitude = convertDegMinToDecDeg(gps.longitude);   
        LonDeg = getDegrees(gps.longitude);
        LonMin = getMinutes(gps.longitude);

        //Serial.print("Lats:");
        //Serial.println(Latitude);
        //Serial.print(LatDeg); Serial.print(" "); Serial.println(LatMin);
        
        Altitude = gps.altitude;
        Distance = dist(Latitude, Longitude);      
        Hour = gps.hour;
        Minute = gps.minute;
        Year = gps.year;
        Month = gps.month;
        Day = gps.day;       
      }
    }     
  }
}

bool Location::hasFix() {
  return gps.fix;
}

// degree-minute format to decimal-degrees
double Location::convertDegMinToDecDeg(float degMin) {
  double min = 0.0;
  double decDeg = 0.0;
 
  //get the minutes, fmod() requires double
  min = fmod((double)degMin, 100.0);
 
  //rebuild coordinates in decimal degrees
  degMin = (int) ( degMin / 100 );
  decDeg = degMin + ( min / 60.00 );
 
  return decDeg;
}


// Gets degrees from a decimal format consisting of degrees and minutes
int Location::getDegrees(float degMin)
{
  return degMin/100;
}

// Gets minutes from a decimal format consisting of degrees and minutes
float Location::getMinutes(float degMin)
{
  return (degMin - (getDegrees(degMin) * 100));
}


double Location::dist(double lat2, double lon2)
{
  double dx, dy, dz;
  double lat1 = targetLat;
  double lon1 = targetLon;
  
  lon1 -= lon2;
  lon1 *= TO_RAD, lat1 *= TO_RAD, lat2 *= TO_RAD;
 
  dz = sin(lat1) - sin(lat2);
  dx = cos(lon1) * cos(lat1) - cos(lat2);
  dy = sin(lon1) * cos(lat1);
  return asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * RADIUS_MI;
}

void Location::setTarget(float lat, float lon)
{
  targetLat = lat;
  targetLon = lon;
}

