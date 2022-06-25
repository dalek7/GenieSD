#ifndef DDTIME_h
#define DDTIME_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


// Originally written by Korneliusz Jarzebski, www.jarzebski.pl
// Modified by Seung-Chan Kim @SKKU around 1620109951627
// Most of codes are from DS1307 : A Class for the DS1307 Real-Time Clock
// Converting unix time from Android : millis --> unsigned long long


#ifndef RTCDATETIME_STRUCT_H
#define RTCDATETIME_STRUCT_H
struct RTCDateTime
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t dayOfWeek;
    uint32_t unixtime;
};
#endif

const uint8_t daysArray [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };
//const uint8_t dowArray[] PROGMEM = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };

/*
RTCDateTime convert_unixtime_to_human_readable(unsigned long unixtime, int consider_timezone=0)
{
    
    unsigned long  t1 = (unixtime +  (unsigned long) 60*60*consider_timezone); // to discard milli-seconds
    
    unsigned long t = (uint32_t) t1;
    
    t -= 946681200;

    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    
    second = t % 60;
    t /= 60;

    minute = t % 60;
    t /= 60;

    hour = t % 24;
    uint16_t days = t / 24;
    uint8_t leap;

    for (year = 0; ; ++year)
    {
        leap = year % 4 == 0;
        if (days < 365 + leap)
        {
            break;
        }
        days -= 365 + leap;
    }

    for (month = 1; ; ++month)
    {
        uint8_t daysPerMonth = pgm_read_byte(daysArray + month - 1);

        if (leap && month == 2)
        {
            ++daysPerMonth;
        }

        if (days < daysPerMonth)
        {
            break;
        }
        days -= daysPerMonth;
    }

    day = days + 1;
    
    //setDateTime(year+2000, month, day, hour, minute, second);
    RTCDateTime dt;
    dt.year = 2000+year;
    dt.month = month;
    dt.day = day;
    dt.hour = hour;
    dt.minute = minute;
    dt.second = second;
    dt.dayOfWeek = 0;
    dt.unixtime = t;//946681200
    
    return dt;
}
*/


RTCDateTime convert_unixtime_to_human_readable(unsigned long long unixtime, int consider_timezone=0)
{
    
    unsigned long long t1 = (unixtime +  (unsigned long long) 1000*60*60*consider_timezone) / 1000L; // to discard milli-seconds
    
    unsigned long t = (uint32_t) t1;
    
    t -= 946681200;

    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    
    second = t % 60;
    t /= 60;

    minute = t % 60;
    t /= 60;

    hour = t % 24;
    uint16_t days = t / 24;
    uint8_t leap;

    for (year = 0; ; ++year)
    {
        leap = year % 4 == 0;
        if (days < 365 + leap)
        {
            break;
        }
        days -= 365 + leap;
    }

    for (month = 1; ; ++month)
    {
        uint8_t daysPerMonth = pgm_read_byte(daysArray + month - 1);

        if (leap && month == 2)
        {
            ++daysPerMonth;
        }

        if (days < daysPerMonth)
        {
            break;
        }
        days -= daysPerMonth;
    }

    day = days + 1;
    
    //setDateTime(year+2000, month, day, hour, minute, second);
    RTCDateTime dt;
    dt.year = 2000+year;
    dt.month = month;
    dt.day = day;
    dt.hour = hour;
    dt.minute = minute;
    dt.second = second;
    dt.dayOfWeek = 0;
    dt.unixtime = t;//946681200
    
    return dt;
}

void convert_to_buf(RTCDateTime rtc, char* buf)
{
    if(buf)
    {
        sprintf(buf, "%d_%02d_%02d %02d:%02d:%02d", rtc.year, rtc.month, rtc.day, rtc.hour, rtc.minute, rtc.second);
    }
      
}

// https://forum.arduino.cc/index.php?topic=378359.0
inline char* ullToStr(const  long long num, char *str){
  uint8_t i = 0;
   long long n = num;
 
  do {
    i++;
  } while ( n /= 10 );
 
  str[i] = '\0';
  n = num;
  
  do {
    str[--i] = ( n % 10 )+'0';
  } while ( n /= 10 );
  
  return str;
}



// https://forum.arduino.cc/index.php?topic=402059.0
inline long long strToUll(const char* ptr) {
   long long result = 0;
  while (*ptr && isdigit(*ptr)) {
    result *= 10;
    result += *ptr++ - '0';
  }
  return result;
}


#endif
