#include "location.h"
#define TOTAL_LOCATIONS 6
//                          Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
byte calendarMonth[]     = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
byte calendarMonthLeap[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // I'm not a proud man...

const TIME_AND_PLACE parkandride = {37.81220277777778, 121.9916, 16, 05, 21};
const TIME_AND_PLACE blackbear = {37.81001388888889, 121.99140277777778, 16, 05, 21};
const TIME_AND_PLACE starbucks = {37.810325, 121.99010833333334, 16, 05, 21};
const TIME_AND_PLACE navlets = {37.80926944444444, 121.98978611111112, 16, 05, 21};
const TIME_AND_PLACE gasstation = {37.80937222222222, 121.99119166666667, 16, 05, 21};
const TIME_AND_PLACE house = {37.810091666666665, 121.98774444444444, 16, 05, 21};

const TIME_AND_PLACE rachels0 = {37.810236, 121.987419, 16, 06, 03};
const TIME_AND_PLACE rachels1 = {37.810089, 121.987394, 16, 06, 03};
const TIME_AND_PLACE rachels2 = {37.809933, 121.987419, 16, 06, 03};

//TIME_AND_PLACE tps[TOTAL_LOCATIONS] = {parkandride, blackbear, starbucks, navlets, gasstation, house};
TIME_AND_PLACE tps[TOTAL_LOCATIONS] = {rachels0, rachels1, rachels2, rachels1, rachels2, rachels0};
uint8_t tp_index = 0;

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

  utcOffset = TIME_OFFSET; 
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

bool Location::TPIndexReady()
{
  bool result = false;
  TIME_AND_PLACE currentTp = tps[tp_index];
  Serial.println("check - ready for adventure?");
  if(Year >= currentTp.year && Month >= currentTp.month && Day >= currentTp.day)
  {
    result = true;
  }
  // Serial.print("Time for adventure: "); Serial.print(result);
  //Serial.print("adventure: "); Serial.print(currentTp);
  return result;
}


void Location::collectGPS()
{
  int tempDay;
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
            
        Minute = gps.minute;
        Hour = (24 + gps.hour + utcOffset)%24;

        // **********************              WHAT DAY IS IT?!?!?!?!!          **********************
        if(utcOffset < 0)
        {
          // negative offset
          
          if(gps.hour < (-1 * utcOffset))
          {
            // gps.hour has already rolled over. (e.g. utc time is 3AM, but offset is -4 means it's 11PM the day before!
            // we can just decrement a day, unless it's the first day of the UTC month, in which case we need to know the days in a month
            if(gps.day == 1)
            {
               // UTC next day
              if(gps.year%4)
              {
                tempDay = calendarMonthLeap[(gps.month - 1) - 1];
              }
              else
              {
                tempDay = calendarMonth[(gps.month - 1) - 1];
              }

              Month = (gps.month == 1) ? 12 : gps.month - 1;
              Year = (gps.month == 1) ? gps.year - 1 : gps.year;
            }
            else
            {
              tempDay = gps.day - 1;
              Month = gps.month;
              Year = gps.year;
            }
          }
          else
          {
            tempDay = gps.day;
            Month = gps.month;
            Year = gps.year;
          }
        }
        else if (utcOffset > 0)
        {
          if (gps.hour + utcOffset > 24)
          {
            if(gps.year%4)
            {
              // leap year!
              if(gps.day == calendarMonthLeap[gps.month -1])
              {
                // last day of the utc month, 1st day local time
                tempDay = 1;
              }
              else
              {
                tempDay = gps.day + 1;
              }
            }
            else
            {
              if(gps.day == calendarMonth[gps.month -1])
              {
                // last day of the utc month, 1st day local time
                tempDay = 1;
              }
              else
              {
                tempDay = gps.day + 1;
              }
            }

            Month = (gps.month == 12) ? 1 : (gps.month + 1);
            Year = (gps.month == 12) ? gps.year + 1 : gps.year;
          }
          else
          {
            tempDay = gps.day + 1;
            Month = gps.month;
            Year = gps.year;
          }
        }
        else
        {
          tempDay = gps.day;
          Month = gps.month;
          Year = gps.year;
        }  

        Day = tempDay;
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

void Location::setTarget()
{
  TIME_AND_PLACE t = tps[tp_index];
  
  targetLat = t.latitude;
  targetLon = t.longitude;
}

