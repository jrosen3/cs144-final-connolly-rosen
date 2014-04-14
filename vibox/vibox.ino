#include <Wire.h>

#define DELAY 0
#define MOTION_PENALTY 1000
#define SOUND_PENALTY 10
#define TEMP_PENALTY 250

int motionPin = 2;
int soundPin = A0;
int tempPin = 0x48; // pin A4 (SDA) and A5 (SCL)

double motionAve = 1; // 1 is empty room
double soundAve = 0; // 9.872 is quite room
double tempAve = 25; // 25 celcious is room temp

int timeNow = 0;



void setup() {
  Serial.begin(9600);
  Wire.begin(); 
  
  pinMode(motionPin, INPUT);
  pinMode(soundPin, INPUT);
  
  timeNow = millis();
}


void loop() {
  
  if ((millis() - timeNow) >= DELAY) {
    // get vals
    int motionVal = digitalRead(motionPin);
//    double soundVal = 16.801 * log(analogRead(soundPin)) + 9.872;
    double soundVal = analogRead(soundPin);
    double tempVal = getTemperature(tempPin);
    
    // update aves
    motionAve = (motionVal + (MOTION_PENALTY * motionAve)) / (MOTION_PENALTY + 1);
    if (soundVal > 0) {
      soundAve = (soundVal + (SOUND_PENALTY * soundAve)) / (SOUND_PENALTY + 1);
    } else {
      soundAve = (0 + ((SOUND_PENALTY * 2) * soundAve)) / ((SOUND_PENALTY * 2) + 1);
    }
    tempAve = (tempVal + (TEMP_PENALTY * tempAve)) / (TEMP_PENALTY + 1);
    
    // print
    Serial.println(motionAve);
    Serial.println(soundAve);
    Serial.println(tempAve);
    
    // update time
    timeNow = millis();
  }
  
}





float getTemperature(int tp){
  Wire.requestFrom(tp, 2); 

  byte MSB = Wire.read();
  byte LSB = Wire.read();

  //it's a 12bit int, using two's compliment for negative
  int TemperatureSum = ((MSB << 8) | LSB) >> 4; 

  double celsius = TemperatureSum * 0.0625;
  return celsius;
}
