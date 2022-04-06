#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Arduino_LSM6DS3.h>
#include <HardwareBLESerial.h>
#include <ArduinoBLE.h>



HardwareBLESerial &bleSerial = HardwareBLESerial::getInstance();

uint16_t BNO055_SAMPLERATE_DELAY_MS = 100;
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

float roll;      //Loft Angle
float pitch;     //Lie Angle
float yaw;       //Face Angle
float velo = 0;  //Velocity

float xz = 0;   //Zeroing Values
float yz = 0;
float zz = 0;
float thresh = 10;  //Hit threshold

int sensor_test = 0;  //Flags
int fitting_mode = 0;
int hit = 0;
int orient_test = 1;
int zero = 0;

int cnt = 0;    //Temporary "Hit detection"

double xPos = 0, yPos = 0;

double ACCEL_VEL_TRANSITION =  (double)(BNO055_SAMPLERATE_DELAY_MS) / 1000.0;
double ACCEL_POS_TRANSITION = 0.5 * ACCEL_VEL_TRANSITION * ACCEL_VEL_TRANSITION;
double DEG_2_RAD = 0.01745329251; //trig functions require radians, BNO055 outputs degrees

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
      /*
      init bluetooth serial device
      */
  if (!bleSerial.beginAndSetupBLE("Instrumented Smart Club")) { //Start BLE
    while (true) {
      Serial.println("failed to initialize HardwareBLESerial!");
      delay(1000);
    }
  }
      /*
      init adafruit BNO055 orientation senesor
      */   
  if (!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  bleSerial.poll();
  if (bleSerial.available() > 0) {  //If data available
    char line = bleSerial.read();
    Serial.print("Received: ");
    Serial.println(line);
    if ((int)line == 48) {  //"0" cast to int (Ascii)
      fitting_mode = 1;
    }
    Serial.print("fitting_mode: ");
    Serial.println(fitting_mode);
  }
    
  if (fitting_mode == 1) {
    Serial.println("Entered Fitting Mode");
    sensors_event_t orientationData, linearAccelData;
    bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
    bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
          
    xz = orientationData.orientation.x;
    yz = orientationData.orientation.y;
    zz = orientationData.orientation.z;
    
  }
  while (fitting_mode == 1) {
    sensors_event_t orientationData, linearAccelData;
    bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
    bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);

    xPos = xPos + ACCEL_POS_TRANSITION * linearAccelData.acceleration.x;
    yPos = yPos + ACCEL_POS_TRANSITION * linearAccelData.acceleration.y;
  
    cnt ++;
    velo = ACCEL_VEL_TRANSITION * linearAccelData.acceleration.y / cos(DEG_2_RAD * orientationData.orientation.y);
    if ((velo > 2) and ((orientationData.orientation.x - xz) < thresh)) { //HIT, Temperaily uses a count
      cnt = 0;
      Serial.println("HIT");
      pitch = orientationData.orientation.z - zz; //Loft
      roll = orientationData.orientation.y - yz;  //Lie
      yaw = orientationData.orientation.x - xz;   //Face
      //bleSerial.write(pitch, ',', roll, ',', yaw, ',', velo);
      //bleSerial.print(pitch, ',', roll, ',', yaw, ',', velo);
      bleSerial.write(pitch);
      bleSerial.print("LOFT:  ");
      bleSerial.println(pitch);
      bleSerial.write(roll);
      bleSerial.print("LIE:  ");
      bleSerial.println(roll);
      bleSerial.write(yaw);
      bleSerial.print("FACE:  ");
      bleSerial.println(yaw);
      bleSerial.write(velo);   //Send Values to app and print values
      bleSerial.print("SPEED:  ");
      bleSerial.println(velo);
      bleSerial.flush();
      
      hit = 0;
      fitting_mode = 0;
      }
    }
  }
