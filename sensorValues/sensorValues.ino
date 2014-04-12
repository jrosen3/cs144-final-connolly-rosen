int motionPin = 2; //digital 2
int soundPin = A0;
double motionAve = 1;

#define PENALTY 1000

void setup(){
 Serial.begin(9600); 
 pinMode(motionPin, INPUT);
 pinMode(soundPin, INPUT);
}


void loop(){
  int motionVal = digitalRead(motionPin);
  motionAve = (motionVal + (PENALTY * motionAve)) / (PENALTY + 1);
  int soundVal = analogRead(soundPin);
  double soundDb = 16.801 * log(soundVal) + 9.872;
  
  if(soundDb > 0) {
    Serial.println(soundDb);
  }
  
  Serial.println(motionAve);

  /*if(motionVal == LOW){ //was motion detected
    delay(1000); 
  }*/
  // delay(1000);

}
