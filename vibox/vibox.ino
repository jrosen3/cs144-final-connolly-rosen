#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>

#define DELAY 0
#define PRINT_DELAY 500
#define MOTION_PENALTY 1000
#define SOUND_PENALTY 100
#define TEMP_PENALTY 250

int motionPin = 2;
int soundPin = A0;
int tempPin = 0x48; // pin A4 (SDA) and A5 (SCL)

double motionAve = 0; // 0 is empty room
double soundAve = 0; // 0 is a quiet room
double tempAve = 25; // 25 Celcius is room temp

int timeNow1 = 0; // loop delay
int timeNow2 = 0; // print delay

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
  
  timeNow1 = millis();
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
      soundAve = (0 + ((SOUND_PENALTY * 250) * soundAve)) / ((SOUND_PENALTY * 250) + 1);
    }
    tempAve = (tempVal + (TEMP_PENALTY * tempAve)) / (TEMP_PENALTY + 1);
    
    // print
    if(millis() - timeNow1 > PRINT_DELAY) {
      Serial.print("M: ");
      Serial.println(motionAve);
      Serial.print("S: ");
      Serial.println(soundAve);
      Serial.print("T: ");
      Serial.println(tempAve);
      Serial.print("Cluster: ");
      Serial.println(calcVibeScore(tempAve, soundAve, motionAve));
      Serial.println();
      timeNow1 = millis();
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
          client.println(",");
          
          client.print("\"vibecluster\":");
          client.print(calcVibeScore(tempAve, soundAve, motionAve));
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

int calcVibeScore(double temp, double sound, double motion) {
  /*
  double c0[3] = {-0.0279306972, -7.98754506, -2.19056361};
  double c1[3] = {-0.334152875, -2.75034960, -0.922019856};
  double c2[3] = {-0.150315079, 0.127898134, -1.47427606};
  double c3[3] = {0.514941983, 5.06378566, -1.26382185};
  double c4[3] = {-0.802725327, 0.334113159, 2.74354896};
  */
  double c0[3] = {-1.0345, -4.2333, -.4778};
  double c1[3] = {-0.088, 4.357, -.6339};
  double c2[3] = {-1.475, 28.122, -1.4508};
  double c3[3] = {2.0739, 11.311, -2.369};
  double c4[3] = {-.328, -16.126, 3.777};
  
  double* ca[] = {c0, c1, c2, c3, c4};
  
//  double in[] = {2.69007718, 5.26042703, 1.74654135, -15.94263692, -19.00936246};
//  double in[] = {22.424, .269, 17.569, -50.811, -1.625};
  double in[] = {29.424, .19, 17.569, -55.811, -1.625};
  
  int p = -1000000;
  int cls = -1;
  for(int i = 0; i++, i<5;) {
    int tp = in[i] + (ca[i][0]*temp) + (ca[i][1]*sound) + (ca[i][2]*motion);
    if(tp > p) {
      p = tp;
      cls = i;
    }
  }
  return cls;
}
