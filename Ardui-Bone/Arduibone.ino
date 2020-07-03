#include<Wire.h>
#include <math.h>
#include "pitches.h"
    
    // defines pins numbers
    
const int trigPin = 3;
const int echoPin = 2;
// defines variables
long duration;
int distance;
int micPin = A0;    // microphone sensor input

int micValue = 0;

const int MPU=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
double pitch,roll,yaw;
double previousYaw, previousRoll, previousPitch;
int count;

int octave = 1;
int pitchtable[3][7] = {
      {NOTE_G2,NOTE_F2,NOTE_E2,NOTE_D2,NOTE_C2,NOTE_B1,NOTE_A1},
      {NOTE_G3,NOTE_F3,NOTE_E3,NOTE_D3,NOTE_C3,NOTE_B2,NOTE_A2},
      {NOTE_G4,NOTE_F4,NOTE_E4,NOTE_D4,NOTE_C4,NOTE_B3,NOTE_A3},
    };

void setup() {
    
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    Wire.begin();
    Wire.beginTransmission(MPU);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission(true);
    Serial.begin(9600); // Starts the serial communication
    count = 0;
}
void loop() {
  
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance= duration*0.034/2;
    // Prints the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.println(distance);
    //tone(4,123);
    
    micValue = analogRead(micPin);  
    micValue = constrain(micValue, 0, 1000); //set sound detect clamp 0-100  
    
    Serial.print("incoming value from microphone =");
    Serial.println( micValue);

    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU,14,true);
    
    int AcXoff,AcYoff,AcZoff,GyXoff,GyYoff,GyZoff;
    int temp,toff;
    double t,tx,tf;
    
    //Acceleration data correction
    AcXoff = -950;
    AcYoff = -300;
    AcZoff = 0;
    
    //Temperature correction
    toff = -1600;
    
    //Gyro correction
    GyXoff = 480;
    GyYoff = 170;
    GyZoff = 210;
    
    //read accel data
    AcX=(Wire.read()<<8|Wire.read()) + AcXoff;
    AcY=(Wire.read()<<8|Wire.read()) + AcYoff;
    AcZ=(Wire.read()<<8|Wire.read()) + AcZoff;
    
    //read temperature data
    temp=(Wire.read()<<8|Wire.read()) + toff;
    tx=temp;
    t = tx/340 + 36.53;
    tf = (t * 9/5) + 32;
    
    //read gyro data
    GyX=(Wire.read()<<8|Wire.read()) + GyXoff;
    GyY=(Wire.read()<<8|Wire.read()) + GyYoff;
    GyZ=(Wire.read()<<8|Wire.read()) + GyZoff;
    
    //get pitch/roll
    getAngle(AcX,AcY,AcZ);
    
    //send the data out the serial port
//    Serial.print("Angle: ");
//    Serial.print("Pitch = "); Serial.print(pitch);
//    Serial.print(" | Yaw = ");Serial.print(yaw);
//    Serial.print(" | Roll = "); Serial.println(roll);
//    
//    Serial.print("Temp: ");
//    Serial.print("Temp(F) = "); Serial.print(tf);
//    Serial.print(" | Temp(C) = "); Serial.println(t);
//    
//    Serial.print("Accelerometer: ");
//    Serial.print("X = "); Serial.print(AcX);
//    Serial.print(" | Y = "); Serial.print(AcY);
//    Serial.print(" | Z = "); Serial.println(AcZ);
//    
//    Serial.print("Gyroscope: ");
//    Serial.print("X = "); Serial.print(GyX);
//    Serial.print(" | Y = "); Serial.print(GyY);
//    Serial.print(" | Z = "); Serial.println(GyZ);
//    Serial.println(" ");

    if(count < 1){
      previousYaw = yaw;
      previousRoll = roll;
      previousPitch = pitch;
      count = 1;
    }

//    if(abs(abs(yaw)-abs(previousYaw)) > 20){
//      Serial.print("Yaw: ");
//      Serial.print(yaw);
//      Serial.print("| PreviousYaw");
//      Serial.print(previousYaw);
//      Serial.print("| Calculation: ");
//      Serial.print(abs(yaw-previousYaw));
//      Serial.println("Tap");
//      
//    }

    if(pitch > 15 ){
      //Serial.println("octave up");
      octave = 2;
    }else if (pitch < -15){
      //Serial.println("octave down");
      octave = 0;
    }else{
      //Serial.println("octave medium");
      octave = 1;
    }

    if(micValue > 200){
        if(distance > 0 && distance < 10){
          tone(4,pitchtable[octave][0]);
        }else if (distance > 10 && distance < 19){
          tone(4,pitchtable[octave][1]);
        }else if (distance > 19 && distance < 28){
          tone(4,pitchtable[octave][2]);
        }else if (distance > 28 && distance < 37){
          tone(4,pitchtable[octave][3]);
        }else if (distance > 37 && distance < 46){
          tone(4,pitchtable[octave][4]);
        }else if (distance > 46 && distance < 55){
          tone(4,pitchtable[octave][5]);
        }else if (distance > 55 && distance < 64){
          tone(4,pitchtable[octave][6]);
        }
    }else{
      noTone(4);
    }
    

    delay(100);

}

void getAngle(int Vx,int Vy,int Vz) {
  double x = Vx;
  double y = Vy;
  double z = Vz;
  
  pitch = atan(x/sqrt((y*y) + (z*z)));
  roll = atan(y/sqrt((x*x) + (z*z)));
  yaw = atan(z/sqrt((x*x) + (z*z)));
  //convert radians into degrees
  pitch = pitch * (180.0/3.14);
  roll = roll * (180.0/3.14);
  yaw = yaw * (180.0/3.14);
}
