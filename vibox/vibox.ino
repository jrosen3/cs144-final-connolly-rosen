#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>

#define DELAY 0
#define MOTION_PENALTY 1000
#define SOUND_PENALTY 10
#define TEMP_PENALTY 250

int motionPin = 2;
int soundPin = A0;
int tempPin = 0x48; // pin A4 (SDA) and A5 (SCL)

double motionAve = 0; // 0 is empty room
double soundAve = 0; // 0 is a quiet room
double tempAve = 25; // 25 Celcius is room temp

int timeNow = 0;
int timeNow2 = 0;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0x98, 0x4F, 0xEE, 0x00, 0x64, 0x6A };
IPAddress ip(192,168,1,177);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);


void setup() {
  Serial.begin(9600);
  Wire.begin(); 
  
  pinMode(motionPin, INPUT);
  pinMode(soundPin, INPUT);
  
  timeNow = millis();
  timeNow2 = millis();
  
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
}


void loop() {
  
  if ((millis() - timeNow2) >= DELAY) {
    // get vals
    int motionVal = 1 - digitalRead(motionPin);
    // double soundVal = 16.801 * log(analogRead(soundPin)) + 9.872;
    double soundVal = analogRead(soundPin);
    double tempVal = getTemperature(tempPin);
    
    // update aves
    motionAve = (motionVal + (MOTION_PENALTY * motionAve)) / (MOTION_PENALTY + 1);
    if (soundVal > 0) {
      soundAve = (soundVal + (SOUND_PENALTY * soundAve)) / (SOUND_PENALTY + 1);
    } else {
      soundAve = (0 + ((SOUND_PENALTY * 5) * soundAve)) / ((SOUND_PENALTY * 5) + 1);
    }
    tempAve = (tempVal + (TEMP_PENALTY * tempAve)) / (TEMP_PENALTY + 1);
    
    // print
    if(millis() - timeNow > 500) {
      Serial.println(motionAve);
      Serial.println(soundAve);
      Serial.println(tempAve);
      // update time
      timeNow = millis();
    }
   
    timeNow2 = millis();
  } // end delay loop
  
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Access-Control-Allow-Origin: *");

          client.println("Connection: close");
          client.println();

          client.print("{\"temperature\":");
          client.print(tempAve);
          client.print(",");
          
          client.print("\"motion\":");
          client.print(motionAve);
          client.print(",");
          
          client.print("\"sound\":");
          client.print(soundAve);
          client.println("}");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
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
