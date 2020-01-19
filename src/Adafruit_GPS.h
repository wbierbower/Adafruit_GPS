/**************************************************************************/
/*!
  @file Adafruit_GPS.h

  This is the Adafruit GPS library - the ultimate GPS library
  for the ultimate GPS module!

  Tested and works great with the Adafruit Ultimate GPS module
  using MTK33x9 chipset
      ------> http://www.adafruit.com/products/746
  Pick one up today at the Adafruit electronics shop
  and help support open source hardware & software! -ada

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada  for Adafruit Industries.
  BSD license, check license.txt for more information
  All text above must be included in any redistribution
*/
/**************************************************************************/

// Fllybob added lines 34,35 and 40,41 to add 100mHz logging capability

#ifndef _ADAFRUIT_GPS_H
#define _ADAFRUIT_GPS_H

/**************************************************************************/
/**
 Comment out the definition of NMEA_EXTENSIONS to make the library use as
 little memory as possible for GPS functionality only. The ARDUINO_ARCH_AVR
 test should leave it out of any compilations for the UNO and similar. */
#ifndef ARDUINO_ARCH_AVR
#define NMEA_EXTENSIONS ///< if defined will include more NMEA sentences
#endif

#define USE_SW_SERIAL ///< comment this out if you don't want to include
                      ///< software serial in the library
#define GPS_DEFAULT_I2C_ADDR                                                   \
  0x10 ///< The default address for I2C transport of GPS data
#define GPS_MAX_I2C_TRANSFER                                                   \
  32 ///< The max number of bytes we'll try to read at once
#define GPS_MAX_SPI_TRANSFER                                                   \
  100                     ///< The max number of bytes we'll try to read at once
#define MAXLINELENGTH 120 ///< how long are max NMEA lines to parse?
#define NMEA_MAX_SENTENCE_ID                                                   \
  20 ///< maximum length of a sentence ID name, including terminating 0
#define NMEA_MAX_SOURCE_ID                                                     \
  3 ///< maximum length of a source ID name, including terminating 0

#include "Arduino.h"
#if (defined(__AVR__) || defined(ESP8266)) && defined(USE_SW_SERIAL)
#include <SoftwareSerial.h>
#endif
#include <SPI.h>
#include <Wire.h>
#include <NMEA_data.h>
#include <Adafruit_PMTK.h>

/// type for resulting code from running check()
typedef enum {
  NMEA_BAD = 0,            ///< passed none of the checks
  NMEA_HAS_DOLLAR = 1,     ///< has a dollar sign in the first position
  NMEA_HAS_CHECKSUM = 2,   ///< has a valid checksum at the end
  NMEA_HAS_NAME = 4,       ///< there is a token after the $ followed by a comma
  NMEA_HAS_SOURCE = 10,    ///< has a recognized source ID
  NMEA_HAS_SENTENCE = 20,  ///< has a recognized sentence ID
  NMEA_HAS_SENTENCE_P = 40 ///< has a recognized parseable sentence ID
} nmea_check_t;

/**************************************************************************/
/*!
    @brief  The GPS class
*/
class Adafruit_GPS : public Print {
public:
  bool begin(uint32_t baud_or_i2caddr);

#if (defined(__AVR__) || defined(ESP8266)) && defined(USE_SW_SERIAL)
  Adafruit_GPS(SoftwareSerial *ser); // Constructor when using SoftwareSerial
#endif
  Adafruit_GPS(HardwareSerial *ser); // Constructor when using HardwareSerial
  Adafruit_GPS(TwoWire *theWire);    // Constructor when using I2C
  Adafruit_GPS(SPIClass *theSPI, int8_t cspin); // Constructor when using SPI

  char *lastNMEA(void);
  boolean newNMEAreceived();
  void common_init(void);

  void sendCommand(const char *);

  void pause(boolean b);

  uint8_t parseHex(char c);

  char read(void);
  size_t write(uint8_t);
  size_t available(void);

  boolean check(char *nmea);
  boolean parse(char *);
  void addChecksum(char *buff);
  nmea_float_t secondsSinceFix();
  nmea_float_t secondsSinceTime();
  nmea_float_t secondsSinceDate();
  void resetSentTime();
  
  boolean wakeup(void);
  boolean standby(void);

  int thisCheck = 0; ///< the results of the check on the current sentence
  char thisSource[NMEA_MAX_SOURCE_ID] = {
      0}; ///< the first two letters of the current sentence, e.g. WI, GP
  char thisSentence[NMEA_MAX_SENTENCE_ID] = {
      0}; ///< the next three letters of the current sentence, e.g. GLL, RMC
  char lastSource[NMEA_MAX_SOURCE_ID] = {
      0}; ///< the results of the check on the most recent successfully parsed
          ///< sentence
  char lastSentence[NMEA_MAX_SENTENCE_ID] = {
      0}; ///< the next three letters of the most recent successfully parsed
          ///< sentence, e.g. GLL, RMC

  uint8_t hour;          ///< GMT hours
  uint8_t minute;        ///< GMT minutes
  uint8_t seconds;       ///< GMT seconds
  uint16_t milliseconds; ///< GMT milliseconds
  uint8_t year;          ///< GMT year
  uint8_t month;         ///< GMT month
  uint8_t day;           ///< GMT day

  nmea_float_t latitude;  ///< Floating point latitude value in degrees/minutes as
                   ///< received from the GPS (DDMM.MMMM)
  nmea_float_t longitude; ///< Floating point longitude value in degrees/minutes as
                   ///< received from the GPS (DDDMM.MMMM)

  /** Fixed point latitude and longitude value with degrees stored in units of
    1/100000 degrees, and minutes stored in units of 1/100000 degrees.  See pull
    #13 for more details:
    https://github.com/adafruit/Adafruit-GPS-Library/pull/13 */
  int32_t latitude_fixed;  ///< Fixed point latitude in decimal degrees
  int32_t longitude_fixed; ///< Fixed point longitude in decimal degrees

  nmea_float_t latitudeDegrees;  ///< Latitude in decimal degrees
  nmea_float_t longitudeDegrees; ///< Longitude in decimal degrees
  nmea_float_t geoidheight;      ///< Diff between geoid height and WGS84 height
  nmea_float_t altitude;         ///< Altitude in meters above MSL
  nmea_float_t speed;            ///< Current speed over ground in knots
  nmea_float_t angle;            ///< Course in degrees from true north
  nmea_float_t magvariation;     ///< Magnetic variation in degrees (vs. true north)
  nmea_float_t HDOP;     ///< Horizontal Dilution of Precision - relative accuracy of
                  ///< horizontal position
  nmea_float_t VDOP;     ///< Vertical Dilution of Precision - relative accuracy of
                  ///< vertical position
  nmea_float_t PDOP;     ///< Position Dilution of Precision - Complex maths derives a
                  ///< simple, single number for each kind of DOP
  char lat = 'X'; ///< N/S
  char lon = 'X'; ///< E/W
  char mag = 'X'; ///< Magnetic variation direction
  boolean fix;    ///< Have a fix?
  uint8_t fixquality;    ///< Fix quality (0, 1, 2 = Invalid, GPS, DGPS)
  uint8_t fixquality_3d; ///< 3D fix quality (1, 3, 3 = Nofix, 2D fix, 3D fix)
  uint8_t satellites;    ///< Number of satellites in use

  boolean waitForSentence(const char *wait, uint8_t max = MAXWAITSENTENCE,
                          boolean usingInterrupts = false);
  boolean LOCUS_StartLogger(void);
  boolean LOCUS_StopLogger(void);
  boolean LOCUS_ReadStatus(void);

  uint16_t LOCUS_serial;  ///< Log serial number
  uint16_t LOCUS_records; ///< Log number of data record
  uint8_t LOCUS_type;     ///< Log type, 0: Overlap, 1: FullStop
  uint8_t LOCUS_mode;     ///< Logging mode, 0x08 interval logger
  uint8_t LOCUS_config;   ///< Contents of configuration
  uint8_t LOCUS_interval; ///< Interval setting
  uint8_t LOCUS_distance; ///< Distance setting
  uint8_t LOCUS_speed;    ///< Speed setting
  uint8_t LOCUS_status;   ///< 0: Logging, 1: Stop logging
  uint8_t LOCUS_percent;  ///< Log life used percentage

#ifdef NMEA_EXTENSIONS
  // NMEA additional public functions
  char *build(char *nmea, const char *thisSource, const char *thisSentence,
              char ref = 'R');

  // NMEA additional public variables
  char txtTXT[63] = {0}; ///< text content from most recent TXT sentence
  int txtTot = 0;        ///< total TXT sentences in group
  int txtID = 0;         ///< id of the text message
  int txtN = 0;          ///< the TXT sentence number
#endif                   // NMEA_EXTENSIONS

private:
  const char *tokenOnList(char *token, const char **list);
  char *parseStr(char *buff, char *p, int n);
  bool isEmpty(char *pStart);
  void parseTime(char *);
  void parseLat(char *);
  boolean parseLatDir(char *);
  void parseLon(char *);
  boolean parseLonDir(char *);
  boolean parseFix(char *);
  // used by check() for validity tests, room for future expansion
  const char *sources[5] = {"II", "WI", "GP", "GN",
                            "ZZZ"}; ///< valid source ids
  const char *sentences_parsed[5] = {"GGA", "GLL", "GSA", "RMC",
                                     "ZZZ"}; ///< parseable sentence ids
  const char *sentences_known[1] = {
      "ZZZ"}; ///< known, but not parseable sentence ids

  // Make all of these times far in the past by setting them near the middle of
  // the millis() range. Timing assumes that sentences are parsed promptly.
  uint32_t lastUpdate =
      2000000000L; ///< millis() when last full sentence successfully parsed
  uint32_t lastFix = 2000000000L;  ///< millis() when last fix received
  uint32_t lastTime = 2000000000L; ///< millis() when last time received
  uint32_t lastDate = 2000000000L; ///< millis() when last date received
  uint32_t recvdTime =
      2000000000L; ///< millis() when last full sentence received
  uint32_t sentTime = 2000000000L; ///< millis() when first character of last
                                   ///< full sentence received
  boolean paused;

  uint8_t parseResponse(char *response);
#if (defined(__AVR__) || defined(ESP8266)) && defined(USE_SW_SERIAL)
  SoftwareSerial *gpsSwSerial;
#endif
  HardwareSerial *gpsHwSerial;
  TwoWire *gpsI2C;
  SPIClass *gpsSPI;
  int8_t gpsSPI_cs = -1;
  SPISettings gpsSPI_settings =
      SPISettings(1000000, MSBFIRST, SPI_MODE0); // default
  char _spibuffer[GPS_MAX_SPI_TRANSFER]; // for when we write data, we need to
                                         // read it too!
  uint8_t _i2caddr;
  char _i2cbuffer[GPS_MAX_I2C_TRANSFER];
  int8_t _buff_max = -1, _buff_idx = 0;
  char last_char = 0;

  volatile char line1[MAXLINELENGTH]; ///< We double buffer: read one line in
                                      ///< and leave one for the main program
  volatile char line2[MAXLINELENGTH]; ///< Second buffer
  volatile uint8_t lineidx = 0;   ///< our index into filling the current line
  volatile char *currentline;     ///< Pointer to current line buffer
  volatile char *lastline;        ///< Pointer to previous line buffer
  volatile boolean recvdflag;     ///< Received flag
  volatile boolean inStandbyMode; ///< In standby flag
};
/**************************************************************************/

#endif
