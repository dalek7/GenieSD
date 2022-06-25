#include "ddTime.h"
#include "MPU9250min.h"

#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>

#include <SoftwareSerial.h>
SoftwareSerial BTSerial(5, 4);   //bluetooth module Tx:Digital 4 Rx:Digital 5

long t0 = 0;  // relative time mills 
unsigned long long t_abs=0L;
String fname;
String inputString = "";  

// Status
int bMPUFlag = 0;
int bSDFlag = -1;
int sdFileCnt = 1;

File myFile;
// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU9250 accelgyro;

#define SS 6

// EEPROM
struct SensorMode {
  float mode;
  
};

int eeAddress = 0;

SensorMode sMode = {
    0
};

String className;

void setup() { 
  // put your setup code here, to run once: sketch_may10a 20210510
  Wire.begin();
  Serial.begin(9600);
  BTSerial.begin(9600);
  
  // MPU 9250lm                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
  accelgyro.devAddr = 0x69;
  Serial.print(F("Initializing I2C devices..."));
  Serial.println(accelgyro.devAddr, HEX);
  //accelgyro.initialize(MPU9250_ACCEL_FS_8, MPU9250_GYRO_FS_1000); // increase range x4 (but losing sensitivity)

  // EEPROM.get()
  EEPROM.get(eeAddress, sMode);
  //Serial.println(sMode.mode);

  if(isnan(sMode.mode)) // if EEPROM is NaN, set mode1 
  {
    accelgyro.initialize(MPU9250_ACCEL_FS_2, MPU9250_GYRO_FS_250); // increase range x4 (but losing sensitivity)
    Serial.println(F("Sensor Mode #1"));
    Serial.println(F("EEPROM is NaN --> Loading the defalut mode"));
    //BTSerial.println(F("Sensor Mode 1"));
  }
  
  else if(sMode.mode == 1)
  { 
    accelgyro.initialize(MPU9250_ACCEL_FS_2, MPU9250_GYRO_FS_250);
    Serial.println(F("Sensor Mode #1"));
    //BTSerial.println(F("Sensor Mode 1"));
  }
  
  else if(sMode.mode == 2)
  { 
    accelgyro.initialize(MPU9250_ACCEL_FS_4, MPU9250_GYRO_FS_500);
    Serial.println(F("Sensor Mode #2"));
    //BTSerial.println(F("Sensor Mode 2"));
  }
  
  else if(sMode.mode == 3)
  { 
    accelgyro.initialize(MPU9250_ACCEL_FS_8, MPU9250_GYRO_FS_1000);
    Serial.println(F("Sensor Mode #3"));
    //BTSerial.println(F("Sensor Mode 3"));
  }
  
  else if(sMode.mode == 4)
  { 
    accelgyro.initialize(MPU9250_ACCEL_FS_16, MPU9250_GYRO_FS_2000);
    Serial.println(F("Sensor Mode #4"));
    //BTSerial.println(F("Sensor Mode 4"));
  }
  
  I2Cdev::writeByte(accelgyro.devAddr, MPU9250_RA_INT_PIN_CFG, 0x02); //set i2c bypass enable pin to true to access magnetometer
  delay(10);
  //I2Cdev::writeByte(MPU9150_RA_MAG_ADDRESS, 0x0A, 0x01); //enable the magnetometer
  //delay(10);

  int bMPUEnabled = 0;
  // verify connection
  Serial.println(F("Testing device connections..."));
  bMPUEnabled = accelgyro.testConnection();
  Serial.println(bMPUEnabled ? "MPU9250 OK" : "MPU9250 failed");


  // SD Card
  Serial.print(F("Init SD card..."));

  if (!SD.begin(SS))
  {
    Serial.println(F("Init failed!"));
    bSDFlag = 3;//
    //return;
  }
  else
  {
    Serial.println(F("Init done."));
  }
  // bt_serial2, 6/24/2021
  BTSerial.println(F("Init done."));
  
  fname= "default.txt";
  
  // RTC
  t0 = millis();
  
  Serial.println(F("Ready !!" ));
}

int ProcBT();
int ProcSensor();

void loop()
{
  int cmd = -1;
  if (BTSerial.available())
  {
    cmd = ProcBT();
  } 
  
  if(cmd==1) //'a'
  {
      Serial.println(F("Sensor init.."));

      bSDFlag = -1;
      bMPUFlag = 2;

      // bt_serial2, 6/24/2021
      BTSerial.println(F("Sensor init.."));
      
  }
  else if(cmd==2) //'q'
  {
      Serial.println(F("Starting SD.."));
      
      // bt_serial2, 6/24/2021
      BTSerial.println(F("Starting SD.."));
      
      if (bMPUFlag<1)
      {
        Serial.println(F("Init first!"));
      }
      else
      {
        if (bSDFlag==-1)
        {
          bSDFlag = 2;
          t0 = millis();
          Serial.println(F("OK !"));
        }
        else // 중복 초기화 방지
        {
          Serial.println(F("Nothing happened !"));
        }
        
      }
      
  }
  else if(cmd==3) //'w'
  {
      Serial.println(F("Stopping SD.."));
      if(bSDFlag==1)
      {
        bSDFlag = -1;
        sdFileCnt = 1;
        bMPUFlag = 0; // stop sensor !
      
        myFile.close();
        Serial.println(F("File closed...Done ! Sensor Also Stopped !"));

        // bt_serial2, 6/24/2021
        BTSerial.println(F("File Save"));
      }
      else
      {
        Serial.println(":( !");
      }
      
  }

  else if(cmd==4)  // not implemented yet --> 's'
  {
      Serial.println(F("Sensor stop.."));

      if (bMPUFlag>0)
      {  
        Serial.println(F("Sensor Stopped !"));
        bMPUFlag = 0;
      }
      else
        Serial.println(F("Nothing happened !"));
  }

  else if(cmd==5) // sensitivity : mode1
  {
    accelgyro.initialize(MPU9250_ACCEL_FS_2, MPU9250_GYRO_FS_250);
    sMode.mode = 1;
    EEPROM.put(eeAddress, sMode);
    Serial.println(F("Sensor Mode #1"));
    BTSerial.println(F("Sensor Mode #1"));
  }

  else if(cmd==6) // sensitivity : mode2
  {
    accelgyro.initialize(MPU9250_ACCEL_FS_4, MPU9250_GYRO_FS_500);
    sMode.mode = 2;
    EEPROM.put(eeAddress, sMode);
    Serial.println(F("Sensor Mode #2"));
    BTSerial.println(F("Sensor Mode #2"));
  }
  
  else if(cmd==7) // sensitivity : mode3
  {
    accelgyro.initialize(MPU9250_ACCEL_FS_8, MPU9250_GYRO_FS_1000);
    sMode.mode = 3;
    EEPROM.put(eeAddress, sMode);
    Serial.println(F("Sensor Mode #3"));
    BTSerial.println(F("Sensor Mode #3"));
  }

  else if(cmd==8) // sensitivity : mode4
  {
    accelgyro.initialize(MPU9250_ACCEL_FS_16, MPU9250_GYRO_FS_2000);
    sMode.mode = 4;
    EEPROM.put(eeAddress, sMode);
    Serial.println(F("Sensor Mode #4"));
    BTSerial.println(F("Sensor Mode #4"));
  }
  //
  
  if (bMPUFlag > 0)
  {
    ProcSensor();
  
  } 
}

int ProcSensor()
{
    uint8_t buffer_a[6];
    uint8_t buffer_g[6];
    
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    
    long tnow = millis();
    long dt = tnow - t0;
    
    if (bMPUFlag > 1)  //  if pressed 'a' : 2
    {
      //dt = 0;
      bMPUFlag = 1;
    }

    //t0 = tnow;

    I2Cdev::readBytes(accelgyro.devAddr, MPU9250_RA_ACCEL_XOUT_H, 6, buffer_a);
    I2Cdev::readBytes(accelgyro.devAddr, MPU9250_RA_GYRO_XOUT_H, 6, buffer_g);

    //sdDataCnt++;
    if (bSDFlag == 1)
    {
      ax = (((int16_t)buffer_a[0]) << 8) | buffer_a[1];
      ay = (((int16_t)buffer_a[2]) << 8) | buffer_a[3];
      az = (((int16_t)buffer_a[4]) << 8) | buffer_a[5];

      gx = (((int16_t)buffer_g[0]) << 8) | buffer_g[1];
      gy = (((int16_t)buffer_g[2]) << 8) | buffer_g[3];
      gz = (((int16_t)buffer_g[4]) << 8) | buffer_g[5];
  
      if(dt<1 || !buffer_a || !buffer_g)
      {
        Serial.println(F("Something wrong !"));
        return -1;
      }

      if(t_abs> 0L) // if received from ANdroid
      {
        unsigned long long  t1 = t_abs+ (unsigned long long) dt;
        char buf[14]={0,};
        ullToStr(t1, buf);
        myFile.print(buf);
      }
      else
      {
        myFile.print(dt);
      }
      
      
      myFile.print('\t');
      myFile.print(ax);
      myFile.print("\t");
      myFile.print(ay);
      myFile.print("\t");
      myFile.print(az);
      myFile.print("\t");
      myFile.print(gx);
      myFile.print("\t");
      myFile.print(gy);
      myFile.print("\t");
      myFile.println(gz);
      
    }
    
    else if (bSDFlag == 2) // Init SD
    {
     
      if (sdFileCnt > 1) // already opened
      {
        myFile.close();
        Serial.print("Closed.."+fname);
                
        //Serial.println(sdDataCnt);
        
        Serial.println();
      }
     
      else
      {
        Serial.println(F("SD ready..."));
        
        
        myFile = SD.open(fname, O_WRITE | O_CREAT  );
        if(myFile)
        {
          Serial.println("File OK! " + fname);
          
        }
        else
        {
          Serial.println(F("Opening file -->FAILED !"));
        }

    
        //file header
        Serial.print(className);
        myFile.print(className + "\t");
        
        char buf1[32];
        sprintf(buf1, "%cdata collected :\t", 37);
        //sprintf(buf1, "data collected :\t");
        Serial.print(buf1);
        myFile.print(buf1);

        if(t_abs> 0L)
        {
    
          RTCDateTime rtc = convert_unixtime_to_human_readable(t_abs, 8);
      
          convert_to_buf(rtc, buf1);

          char buf_raw[14]={0,};
          ullToStr(t_abs, buf_raw);
        
          myFile.print(buf_raw);
          myFile.print(F("\t"));
          myFile.println(buf1);
          
          Serial.println(buf1);
        }
        else
        {
          myFile.println(" ");
        }
       
        sdFileCnt++;
        //sdDataCnt = 1;
        bSDFlag = 1;
        Serial.println(F("Writing to SD card."));
      }
      
    }
    else if (bSDFlag == -1 || bSDFlag ==3) // print to UART, 3 for pause
    {
      ax = (((int16_t)buffer_a[0]) << 8) | buffer_a[1];
      ay = (((int16_t)buffer_a[2]) << 8) | buffer_a[3];
      az = (((int16_t)buffer_a[4]) << 8) | buffer_a[5];

      gx = (((int16_t)buffer_g[0]) << 8) | buffer_g[1];
      gy = (((int16_t)buffer_g[2]) << 8) | buffer_g[3];
      gz = (((int16_t)buffer_g[4]) << 8) | buffer_g[5];

      Serial.print(dt);
      
      Serial.print(F("\t"));
      Serial.print(ax);
      Serial.print(F("\t"));
      Serial.print(ay);
      Serial.print(F("\t"));
      Serial.print(az);
      Serial.print(F("\t"));
      Serial.print(gx);
      Serial.print(F("\t"));
      Serial.print(gy);
      Serial.print(F("\t"));
      Serial.println(gz);
      
    }
  return 0;
}

int ProcBT()
{
    int ret = -1;
    String data = BTSerial.readString();
    //data.trim();
    //Serial.println("RECV: " + String(data.length())+" CHARS" );
    //Serial.println(data);
    
    int len = data.length();
    //Serial.println(len);
    
    String data1;
   
    if (len>12) 
      data1 = data.substring(0,10+3); //13개만
    else 
      data1 = data;

    Serial.println(data1);
    len = data1.length();
    Serial.println(len);
     char bufymd[20]={0,};
    //if(len==10+3)
    
    if(data1[0]=='C') {
      className = data1;
      //Serial.print("className : ");
      //Serial.println(className);
    } 
    
    if(len>12)
    {
    
      //Serial.println("without ms");
      //t_abs = atol(data1.c_str());
      t_abs = strToUll(data1.c_str());
      /*
      char buf[14]={0,};
      ullToStr(t_abs, buf);
      Serial.println(buf);
      Serial.println("0000");
      */
 
      RTCDateTime rtc = convert_unixtime_to_human_readable(t_abs, 8);
      
      sprintf(bufymd, "%02d%02d%02d",  rtc.year-2000, rtc.month, rtc.day);
      Serial.println(bufymd);
      String dirname = String(bufymd);
      SD.mkdir(dirname);
      Serial.println(dirname);
      
      //char fname_buf[20]={0,}; //8+3+1 + 6+1
      sprintf(bufymd, "%02d%02d%02d/%02d%02d%02d.txt", rtc.year-2000, rtc.month, rtc.day, rtc.hour, rtc.minute, rtc.second);
      fname = String(bufymd);
 
      Serial.println(fname);

      // bt_serial2, 6/24/2021
      BTSerial.println(fname);
      
      t0 = millis();
      ret = 0;
      
    }
    
   
   else if(len==3)
    {
            
      String c = data.substring(1);
      Serial.println(c);
      //c.toCharArray(bufymd, c.length()+1); // why +1 ?? 
      
      int cmd = atoi(c.c_str());
      
      Serial.print(F("CMD : "));
      Serial.println(cmd);

      ret = cmd;
      //Serial.println(" ");  
      
    }

    return ret;
}

// Through UART
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    //inputString = inCharUART;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar='a' || inChar == '\n' || inChar=='1') {
      bMPUFlag = 1;//2
    }
  }
}
